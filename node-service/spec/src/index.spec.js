const debug = require('debug')('api:index');
const request = require('supertest');
const server = require('../../app');
const { User, Device, Observation } = require('../../app/db');

describe('index', () => {
    let DeviceId;

    beforeEach((done) => {
        User.create({ name: 'foo' }).then((user) => {
            return Device.create({ UserId: user.id });
        }).then((device) => {
            DeviceId = device.id;
            done();
        });
    });
    describe('POST /observations', () => {
        it('should create a new observation', (done) => {
            request(server).post(
                '/observations'
            ).send({
                DeviceId, ph: 1.5, temperature: 66.66, humidity: 11.1, conductivity: 0
            }).expect(
                'Content-Type', /json/
            ).expect(200).then((res) => {
                const obs = res.body;

                expect(obs.DeviceId).toBe(DeviceId);
                expect(obs.ph).toBe(1.5);
                done();
            }).catch(done.fail);
        });
    });
    describe('GET /observations', () => {
        it('should return empty list', (done) => {
            request(server).get(
                '/observations'
            ).expect(
                'Content-Type', /json/
            ).expect(200).then((res) => {
                expect(res.body).toEqual([]);
                done();
            }).catch(done.fail);
        });
        it('should return all observations', (done) => {
            Observation.create({ DeviceId, ph: 1.5 }).then(() => {
                return Observation.create({ DeviceId, ph: 2.01 });
            }).then(() => {
                request(server).get(
                    '/observations'
                ).expect(
                    'Content-Type', /json/
                ).expect(200).then((res) => {
                    const obs = res.body[ 0 ];

                    expect(res.body.length).toBe(2);
                    expect(obs.ph).toBe(2.01);
                    done();
                }).catch(done.fail);
            })
        });
        it('should limit observations', (done) => {
            Observation.bulkCreate([
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 }
            ]).then(() => {
                request(server).get(
                    '/observations?limit=2'
                ).expect(
                    'Content-Type', /json/
                ).expect(200).then((res) => {
                    expect(res.body.length).toBe(2);
                    done();
                }).catch(done.fail);
            })
        });
        it('should return nothing, not in date range', (done) => {
            Observation.bulkCreate([
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 }
            ]).then(() => {
                const d = new Date();
                const after = `${d.getUTCFullYear()}-${d.getUTCMonth() + 1}-${d.getUTCDate() - 1}`;
                const before = `${d.getUTCFullYear()}-${d.getUTCMonth() + 1}-${d.getUTCDate()}`

                return request(server).get(
                    `/observations?after=${after}&before=${before}`
                ).expect(
                    'Content-Type', /json/
                ).expect(200).then((res) => {
                    expect(res.body.length).toBe(0);
                    done();
                }).catch(done.fail);
            }).catch(done.fail);
        });
        it('should return data in date range', (done) => {
            Observation.bulkCreate([
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 },
                { DeviceId, ph: 1.5 }
            ]).then(() => {
                const d = new Date();
                const after = `${d.getUTCFullYear()}-${d.getUTCMonth() + 1}-${d.getUTCDate() - 1}`;
                const before = `${d.getUTCFullYear()}-${d.getUTCMonth() + 1}-${d.getUTCDate() + 1}`

                return request(server).get(
                    `/observations?after=${after}&before=${before}`
                ).expect(
                    'Content-Type', /json/
                ).expect(200).then((res) => {
                    expect(res.body.length).toBe(3);
                    done();
                }).catch(done.fail);
            }).catch(done.fail);
        });
    });
});
