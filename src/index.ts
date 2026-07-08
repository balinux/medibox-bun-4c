import { initDb } from "./db/schema";
import {
  getUpcomingMedicines,
  printUpcomingMedicines,
} from "./services/reminder";
import { Hono } from "hono";

// Inisialisasi Database jika belum ada
initDb();

// console.log("Medibox Bun is running...");
// printUpcomingMedicines();

const app = new Hono();
// 1. GET ALL BOOKS (Read)
app.get("/api/schedule", (c) => {
  const medicines = getUpcomingMedicines();
  return c.json(medicines);
});

export default app;
