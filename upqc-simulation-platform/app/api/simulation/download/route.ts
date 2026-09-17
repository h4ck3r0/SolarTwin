import { NextResponse } from 'next/server';
import fs from 'fs';
import path from 'path';

export async function GET() {
  try {
    const filePath = path.join(process.cwd(), '..', 'python_backend', 'full_simulation_results.json');
    if (!fs.existsSync(filePath)) {
      return NextResponse.json({ error: 'No raw data available yet. Please run a simulation first.' }, { status: 404 });
    }
    
    const fileBuffer = fs.readFileSync(filePath);
    
    return new NextResponse(fileBuffer, {
      headers: {
        'Content-Type': 'application/json',
        'Content-Disposition': 'attachment; filename="full_simulation_results.json"',
      },
    });
  } catch (error) {
    return NextResponse.json({ error: 'Failed to download data' }, { status: 500 });
  }
}
