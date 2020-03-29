const { Sequelize, Model, DataTypes } = require('sequelize');

const sequelize = new Sequelize(process.env.DATABASE_URL, {
    dialect: 'postgres',
    protocol: 'postgres',
    dialectOptions: { ssl: process.env.USE_PG_SSL == '1' }
});

class User extends Model {}
User.init({
    name: {
        type: DataTypes.STRING(500),
        defaultValue: ''
    },
    email: {
        type: DataTypes.STRING(500),
        allowNull: false,
        unique: true,
        defaultValue: ''
    },
}, { sequelize, modelName: 'User' });

class Device extends Model {}
Device.init({
    name: {
        type: DataTypes.STRING(500),
        defaultValue: ''
    },
}, { sequelize, modelName: 'Device' });

class Observation extends Model {}
Observation.init({
    temperature: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('temperature')
    },
    humidity: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('humidity')
    },
    ph: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('ph')
    },
    conductivity: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('conductivity')
    },
    waterlevel: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('waterlevel')
    },
    soilmoisture: {
        type: DataTypes.DECIMAL(10, 2),
        get: get('soilmoisture')
    },
}, { sequelize, modelName: 'Observation' });

User.hasMany(Device);
Device.belongsTo(User);
Observation.belongsTo(Device);

function get(key) {
    return function() {
        return parseFloat(this.getDataValue(key));
    }
}

module.exports = {
    sequelize, Observation, User, Device
};