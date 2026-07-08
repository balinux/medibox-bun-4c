import { initDb } from "./db/schema";
import {
  getUpcomingMedicines,
  printUpcomingMedicines,
} from "./services/reminder";
import { Hono } from "hono";
import { cors } from "hono/cors";

// Inisialisasi Database jika belum ada
initDb();

// console.log("Medibox Bun is running...");
// printUpcomingMedicines();

const app = new Hono();

app.use("/*", cors());

app.get("/api/schedule", (c) => {
  const medicines = getUpcomingMedicines();
  return c.json(medicines);
});

export default app;
