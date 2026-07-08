import { db } from "./db";
import { initDb } from "./db/schema";

initDb();

db.exec(`DELETE FROM schedules;`);

const stmt = db.prepare(`
INSERT INTO schedules (
    medicine_name,
    schedule_time,
    box_number
) VALUES (?, ?, ?)
`);

stmt.run("Paracetamol", "13:00", "1");
stmt.run("Vitamin C", "13:15", "2");
stmt.run("Amoxicillin", "13:45", "1");
stmt.run("Omeprazole", "15:00", "2");

console.log("Seed berhasil");
