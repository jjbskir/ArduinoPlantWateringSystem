'use strict';

function getUpdate(body, model) {
    const UPDATE = {};

    if (Object.keys(body).length === 0) {
        return UPDATE;
    }

    Object.keys(
        model.rawAttributes
    ).forEach(param => addParam(UPDATE, body, param));

    return UPDATE;
}

function addParam(update, body, key) {
    const VAL = body[ key ];

    if (VAL) {
        update[ key ] = VAL;
    }
}

const express = require('express');
const { Op } = require('sequelize');
const bodyParser = require('body-parser');
const compression = require('compression');
const debug = require('debug')('api:app');
const { Observation } = require('./db');

const app = express();
const router = express.Router();

// Routes
router.post('/observations', (req, res, next) => {
    const BODY = req.body || {};
    debug('POST /observations', BODY);
    const UPDATE = getUpdate(BODY, Observation);

    return Observation.create(UPDATE).then(obs => {
        return res.json(obs.toJSON());
    }).catch(next);
});
router.get('/observations', (req, res, next) => {
    const where = {};
    const limit = req.query.limit || 24;
    const after = req.query.after ? new Date(req.query.after) : null;
    const before = req.query.before ? new Date(req.query.before) : null;

    if (after || before) {
        where.createdAt = { [Op.gt]: after, [Op.lt]: before };
    }

    debug('GET /observations', where);

    return Observation.findAll({
        limit, where, order: [ [ 'id', 'DESC' ] ]
    }).then(observations => {
        return res.json(observations.map(obs => obs.toJSON()));
    }).catch(next);
});

// middleware
app.use(compression());
app.use(bodyParser.urlencoded());
app.use(bodyParser.json());
app.use(bodyParser.text());
app.use(router);
app.use((err, req, res, next) => {
    debug('Error processing request:', err, err.stack);
    res.status(500).json({ error: { message: 'Internal server error' } });
});

module.exports = app;