#API

##Neccesary environment variables (.env)
```
DATABASE_URL = URL to postgres db i.e. "postgres://user:pass@localhost:5000/"
```

##Setup on Mac
Use local database and cache, otherwise can not run tests.

Install postgresql by downloading http://postgresapp.com/ or
```bash
$ brew install postgresql
```
run cache and database
```bash
$ postgres -D /usr/local/var/postgres
```
Create table and add schemas
```
$ psql -U <username> -c "CREATE DATABASE <name>"
```

##Install

```bash
$ npm install
```

##Run

launch api with local enviorment and debug mode
```bash
$ DEBUG=api:* node -r dotenv/config bin/www
```

##Test

run integration tests against database.
WARNING: this will wipe your database before running. Run against local/test DB.
```bash
$ DEBUG=api:* npm test
```