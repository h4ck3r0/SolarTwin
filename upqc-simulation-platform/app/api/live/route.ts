import { NextResponse } from 'next/server';

/**
 * Next.js API proxy for the LSTM live prediction server.
 * Bridges the frontend (port 3000) to live_server.py (port 8000),
 * avoiding CORS issues when fetching from the browser directly.
 *
 * GET /api/live  →  http://localhost:8000/api/live_data
 */
export async function GET() {
  try {
    const res = await fetch('http://localhost:8000/api/live_data', {
      // No caching — always get the freshest prediction tick
      cache: 'no-store',
      next: { revalidate: 0 },
    });

    if (!res.ok) {
      return NextResponse.json(
        { error: `Live server responded with ${res.status}` },
        { status: 502 }
      );
    }

    const data = await res.json();
    return NextResponse.json(data);
  } catch (err: any) {
    // live_server.py is not running — return a clean error payload
    return NextResponse.json(
      {
        error: 'LSTM live server is offline. Start it with: uvicorn live_server:app --port 8000',
        current: null,
        history: [],
      },
      { status: 503 }
    );
  }
}
