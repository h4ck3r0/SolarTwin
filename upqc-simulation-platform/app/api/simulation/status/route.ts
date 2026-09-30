import { NextResponse } from 'next/server';

/**
 * Phase 4.2 — Proxy GET /api/simulation/status → main.py:8001/status
 * Returns: { running: bool, progress: float (0-1), stage: string, last_duration_s: number | null }
 * Used by the workspace page to show real solver progress instead of a fake timer.
 */
export async function GET() {
  try {
    const res = await fetch('http://127.0.0.1:8001/status', { cache: 'no-store' });
    if (!res.ok) {
      return NextResponse.json({ running: false, progress: 0, stage: 'offline', last_duration_s: null });
    }
    return NextResponse.json(await res.json());
  } catch {
    return NextResponse.json({ running: false, progress: 0, stage: 'offline', last_duration_s: null });
  }
}
