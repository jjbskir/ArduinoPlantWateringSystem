require('dotenv').load({ silent: true });

const sequelize = require('../app/db').sequelize;

beforeEach((done) => {
    sequelize.sync({ force: true }).then(() => done());
});