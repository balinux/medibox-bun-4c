import { initDb } from "./db/schema";
import { printUpcomingMedicines } from "./services/reminder";
import { Hono } from "hono";

// Inisialisasi Database jika belum ada
initDb();

console.log("Medibox Bun is running...");
printUpcomingMedicines();

const app = new Hono();
// 1. GET ALL BOOKS (Read)
app.get("/api/schedule", (c) => {
  return c.json({
    id: 11,
    medicine_name: "Amoxicillin",
    schedule_time: "13:45",
  });
});

export default app;
