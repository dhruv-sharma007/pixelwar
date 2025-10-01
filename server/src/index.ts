import { Hono } from 'hono'
import { signIgUser } from './controllers/user.c'
import mongoose from 'mongoose'
import { db } from './config'

const app = new Hono()

mongoose.connect(db)
  .then(conn => {
    console.log(`MongoDB connected: ${conn.connection.host}`)
  })
  .catch(err => {
    console.error('DB connection failed:', err)
    process.exit(1)
  })

app.post('/sign-in', signIgUser)

app.get('/', (c) => c.text('Hello Hono!'))

export default app
