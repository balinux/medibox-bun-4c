import { db } from "./index";

export function initDb() {
  db.prepare(
    `
    CREATE TABLE IF NOT EXISTS schedules (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        medicine_name TEXT NOT NULL,
        schedule_time TEXT NOT NULL,
        box_number TEXT NOT NULL
    );
  `,
  ).run();
}
