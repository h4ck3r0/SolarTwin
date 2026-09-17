'use client';

import React, { useState, useEffect, useMemo } from 'react';
import {
  ResponsiveContainer, LineChart, Line, XAxis, YAxis,
  CartesianGrid, Tooltip, AreaChart, Area, ReferenceLine,
} from 'recharts';
import { SimulationDataPoint } from '@/lib/simulation-types';
import Link from 'next/link';
import {
  ArrowLeft, Activity, Download, ChevronRight, AlertTriangle,
  Flame, ZapOff, CheckCircle2, TrendingDown, Lightbulb, X,
  ChevronDown, ChevronUp,
} from 'lucide-react';

// ─── Colour helpers ──────────────────────────────────────────────────────────
const STATUS_OK      = { bg: 'bg-emerald-50', border: 'border-emerald-200', text: 'text-emerald-800', dot: 'bg-emerald-400' };
const STATUS_WARN    = { bg: 'bg-amber-50',   border: 'border-amber-200',   text: 'text-amber-800',   dot: 'bg-amber-400'   };
const STATUS_DANGER  = { bg: 'bg-rose-50',    border: 'border-rose-200',    text: 'text-rose-800',    dot: 'bg-rose-500'    };

// ─── Types ────────────────────────────────────────────────────────────────────
interface DiagnosticFix {
  param: string;
  current: string;
  recommended: string;
  why: string;
}

interface Diagnostic {
  id: string;
  severity: 'fatal' | 'warning' | 'info';
  title: string;
  subtitle: string;
  rootCause: string[];
  fixes: DiagnosticFix[];
}

// ─── KPI Card ────────────────────────────────────────────────────────────────
function KpiCard({ label, value, unit, status, sub }:
  { label: string; value: string; unit: string; status: 'ok' | 'warn' | 'danger'; sub?: string }) {
  const c = status === 'ok' ? STATUS_OK : status === 'warn' ? STATUS_WARN : STATUS_DANGER;
  return (
    <div className={`${c.bg} border ${c.border} rounded-xl p-4 flex flex-col gap-1`}>
      <div className="flex items-center gap-2">
        <div className={`w-2 h-2 rounded-full ${c.dot}`} />
        <span className={`text-[10px] font-black uppercase tracking-widest ${c.text} opacity-70`}>{label}</span>
      </div>
      <div className={`text-2xl font-black tracking-tight ${c.text}`}>
        {value}<span className="text-sm font-bold ml-1 opacity-60">{unit}</span>
      </div>
      {sub && <div className={`text-[10px] ${c.text} opacity-60 font-mono`}>{sub}</div>}
    </div>
  );
}

// ─── Diagnostic Panel ────────────────────────────────────────────────────────
function DiagnosticCard({ diag }: { diag: Diagnostic }) {
  const [open, setOpen] = useState(true);
  const c = diag.severity === 'fatal' ? STATUS_DANGER : diag.severity === 'warning' ? STATUS_WARN : STATUS_OK;
  const Icon = diag.severity === 'fatal' ? ZapOff : diag.severity === 'warning' ? TrendingDown : CheckCircle2;

  return (
    <div className={`${c.bg} border ${c.border} rounded-xl overflow-hidden shadow-sm`}>
      {/* Header */}
      <button
        className="w-full flex items-center gap-4 px-6 py-4 text-left"
        onClick={() => setOpen(!open)}
      >
        <div className={`p-2 rounded-lg ${diag.severity === 'fatal' ? 'bg-rose-100' : diag.severity === 'warning' ? 'bg-amber-100' : 'bg-emerald-100'}`}>
          <Icon className={`w-5 h-5 ${c.text}`} />
        </div>
        <div className="flex-1">
          <h3 className={`font-black text-sm tracking-tight ${c.text}`}>{diag.title}</h3>
          <p className={`text-xs ${c.text} opacity-70 mt-0.5`}>{diag.subtitle}</p>
        </div>
        {open ? <ChevronUp className={`w-4 h-4 ${c.text} opacity-50`} /> : <ChevronDown className={`w-4 h-4 ${c.text} opacity-50`} />}
      </button>

      {open && (
        <div className="px-6 pb-5 space-y-4">
          {/* Root Cause Chain */}
          <div>
            <div className={`text-[10px] font-black uppercase tracking-widest ${c.text} opacity-60 mb-2`}>Root Cause Chain</div>
            <div className="space-y-1">
              {diag.rootCause.map((step, i) => (
                <div key={i} className="flex items-start gap-2">
                  <div className={`shrink-0 w-5 h-5 rounded-full ${c.text} ${diag.severity === 'fatal' ? 'bg-rose-100' : 'bg-amber-100'} flex items-center justify-center text-[10px] font-black mt-0.5`}>
                    {i + 1}
                  </div>
                  <p className={`text-xs ${c.text} font-mono leading-relaxed`}>{step}</p>
                </div>
              ))}
            </div>
          </div>

          {/* Fix Table */}
          <div>
            <div className="flex items-center gap-2 mb-2">
              <Lightbulb className={`w-3.5 h-3.5 ${c.text}`} />
              <div className={`text-[10px] font-black uppercase tracking-widest ${c.text} opacity-60`}>Recommended Fixes</div>
            </div>
            <div className="overflow-x-auto rounded-lg border border-white/60">
              <table className="w-full text-xs">
                <thead>
                  <tr className={`${diag.severity === 'fatal' ? 'bg-rose-100/60' : 'bg-amber-100/60'} text-left`}>
                    <th className={`px-3 py-2 font-black ${c.text} text-[10px] uppercase tracking-wide`}>Parameter</th>
                    <th className={`px-3 py-2 font-black ${c.text} text-[10px] uppercase tracking-wide`}>Current</th>
                    <th className={`px-3 py-2 font-black ${c.text} text-[10px] uppercase tracking-wide`}>Recommended</th>
                    <th className={`px-3 py-2 font-black ${c.text} text-[10px] uppercase tracking-wide`}>Physics Reason</th>
                  </tr>
                </thead>
                <tbody>
                  {diag.fixes.map((fix, i) => (
                    <tr key={i} className="border-t border-white/40 bg-white/40">
                      <td className={`px-3 py-2 font-mono font-bold ${c.text}`}>{fix.param}</td>
                      <td className="px-3 py-2 font-mono text-slate-500 line-through">{fix.current}</td>
                      <td className={`px-3 py-2 font-mono font-bold ${c.text}`}>{fix.recommended}</td>
                      <td className="px-3 py-2 text-slate-600 leading-relaxed">{fix.why}</td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          </div>
        </div>
      )}
    </div>
  );
}

// ─── Main Page ────────────────────────────────────────────────────────────────
export default function StatisticsPage() {
  const [data, setData] = useState<SimulationDataPoint[]>([]);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    const stored = localStorage.getItem('simulation_results');
    if (stored) {
      try {
        const parsed = JSON.parse(stored);
        if (Array.isArray(parsed) && parsed.length > 0) setData(parsed);
      } catch (e) { console.error(e); }
    }
    setIsLoading(false);
  }, []);

  const handleExportCSV = () => {
    if (data.length === 0) return;
    const headers = Object.keys(data[0]);
    const rows = data.map((dp) => headers.map((k) => (dp as any)[k]));
    const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...rows.map((e) => e.join(','))].join('\n');
    const link = document.createElement('a');
    link.setAttribute('href', encodeURI(csvContent));
    link.setAttribute('download', 'simulation_subset.csv');
    document.body.appendChild(link); link.click(); document.body.removeChild(link);
  };

  const handleDownloadFullRaw = () => { window.open('/api/simulation/download', '_blank'); };

  // ── Derived diagnostics ──────────────────────────────────────────────────────
  const diags = useMemo<Diagnostic[]>(() => {
    if (data.length === 0) return [];
    const results: Diagnostic[] = [];

    const vdcFirst = data[0].dcLinkVoltage;
    const vdcLast  = data[data.length - 1].dcLinkVoltage;
    const vdcMin   = Math.min(...data.map(d => d.dcLinkVoltage));
    const vdcDrop  = vdcFirst - vdcLast;
    const isDcCrash = vdcMin < 400;
    const isDcDrain = !isDcCrash && vdcDrop > 5;

    const solarPeak = Math.max(...data.map(d => d.solarPowerWatts));
    const loadKw    = 15; // default — could pull from data in future
    const rGrid     = 0.1;
    const vBase     = (415 / Math.sqrt(3)) * Math.sqrt(2);
    const iGridEst  = solarPeak / (1.5 * vBase);
    const pDrainEst = rGrid * iGridEst * iGridEst;

    if (isDcCrash) {
      results.push({
        id: 'dc-crash',
        severity: 'fatal',
        title: 'FATAL — DC-Link Voltage Collapse',
        subtitle: `Vdc dropped below 400V minimum. Physics engine computed Vdc_min = ${vdcMin.toFixed(1)}V`,
        rootCause: [
          `Solar array generates ${(solarPeak/1000).toFixed(1)} kW vs ${loadKw} kW load → ${((solarPeak/1000)-loadKw).toFixed(1)} kW net export`,
          `Net export creates reverse grid current ≈ ${iGridEst.toFixed(0)} A peak through grid impedance`,
          `Series UPQC must inject V_inj = R_grid × I_grid = ${rGrid}Ω × ${iGridEst.toFixed(0)}A = ${(rGrid*iGridEst).toFixed(1)}V per phase`,
          `Power drained from DC capacitor: P_series = V_inj × I_grid = ${(pDrainEst/1000).toFixed(1)} kW — exceeds battery PI response at t=0`,
          `Capacitor discharges: ΔV = ΔQ/C = P_drain × Δt / (C × Vdc) — cannot recover without sufficient battery capacity`,
        ],
        fixes: [
          { param: 'batteryCapacityKwh', current: '100 kWh', recommended: '≥ 500 kWh', why: 'Larger battery gives PI controller enough headroom to inject >200kW during transient' },
          { param: 'gridResistance', current: `${rGrid} Ω`, recommended: '≤ 0.01 Ω', why: `V_inj = R × I_grid. Lower R → smaller voltage injection → less DC power consumed per cycle` },
          { param: 'dcCapacitance', current: '2200 μF', recommended: '≥ 50000 μF', why: 'Larger capacitor slows ΔV/Δt rate. Gives PI controller more time to respond before voltage falls below threshold' },
          { param: 'solarStringsParallel', current: '88', recommended: '≤ 10', why: `Reducing solar power reduces I_grid export, which reduces V_inj and P_drain proportionally (P ∝ I²)` },
        ],
      });
    } else if (isDcDrain) {
      results.push({
        id: 'dc-drain',
        severity: 'warning',
        title: 'WARNING — DC-Link Voltage Draining',
        subtitle: `Vdc: ${vdcFirst.toFixed(1)}V → ${vdcLast.toFixed(1)}V (−${vdcDrop.toFixed(1)}V). PI controller is stabilising but with steady-state error.`,
        rootCause: [
          `Series compensation consumes ≈ ${(pDrainEst/1000).toFixed(1)} kW from DC link each cycle`,
          `Battery PI controller reaches steady-state but with a ${vdcDrop.toFixed(1)}V droop from nominal 700V`,
          `Steady-state droop = P_drain / (kp_dc × C_dc) — PI gain too low to eliminate error`,
        ],
        fixes: [
          { param: 'kp (PI gain)', current: '0.5', recommended: '5.0 – 10.0', why: 'Higher Kp forces PI controller to drive Vdc error to zero faster, eliminating steady-state droop' },
          { param: 'ki (PI integral)', current: '10', recommended: '50 – 100', why: 'Higher Ki eliminates steady-state error. Integral term accumulates until Vdc = 700V exactly' },
          { param: 'gridResistance', current: `${rGrid} Ω`, recommended: '≤ 0.05 Ω', why: 'Lower grid impedance reduces power consumed by series compensator' },
        ],
      });
    }

    const maxIgbt = Math.max(...data.map(d => d.igbtTemperature || 0));
    if (maxIgbt > 125) {
      results.push({
        id: 'thermal',
        severity: 'fatal',
        title: 'THERMAL OVERLOAD — IGBT Junction Critical',
        subtitle: `T_junction peak = ${maxIgbt.toFixed(1)}°C exceeds Si IGBT limit of 125°C`,
        rootCause: [
          `High shunt injection current → conduction losses P_cond = V_CE × I_rms`,
          `Thermal resistance R_th(j-c) limits heat dissipation from junction to case`,
          `With ambient at ${data[0].solarTemperature?.toFixed(0) || '25'}°C: T_j = T_amb + P_loss × R_th(j-a) = ${maxIgbt.toFixed(1)}°C`,
        ],
        fixes: [
          { param: 'solarTemperature', current: `${data[0].solarTemperature?.toFixed(0) || '25'}°C`, recommended: '≤ 35°C (ambient)', why: 'Lower ambient directly reduces T_junction by same delta. Real fix: add forced air or liquid cooling' },
          { param: 'loadActivePower', current: '15 kW', recommended: 'Derate by 20%', why: 'Reducing load current lowers I²R switching losses inside IGBT module' },
          { param: 'filterInductance', current: '2.5 mH', recommended: '5–10 mH', why: 'Higher inductance reduces current ripple → lower peak IGBT current → lower switching losses' },
        ],
      });
    }

    const minGridV = Math.min(...data.map(d => Math.abs(d.gridVoltageA)));
    if (minGridV < 150) {
      results.push({
        id: 'grid-sag',
        severity: 'warning',
        title: 'GRID FAULT — Severe Voltage Sag',
        subtitle: `Minimum measured grid voltage = ${minGridV.toFixed(1)}V (peak). Series UPQC may be undersized.`,
        rootCause: [
          `Grid voltage sag: V_terminal = V_source − R_grid × I_grid`,
          `With high export current the terminal voltage can sag below compensation range`,
          `Series injector rating must exceed the sag depth to restore nominal voltage`,
        ],
        fixes: [
          { param: 'gridReactance', current: '0.2 Ω', recommended: '≤ 0.05 Ω', why: 'Lower grid impedance reduces sag magnitude at PCC' },
          { param: 'filterInductance', current: '2.5 mH', recommended: '5 mH', why: 'Improves current waveform quality, reduces peak grid current that causes sag' },
          { param: 'isGridConnected', current: 'true', recommended: 'Check tie-line capacity', why: 'If grid is weak (high impedance), islanded operation may give better voltage stability' },
        ],
      });
    }

    if (results.length === 0) {
      results.push({
        id: 'healthy',
        severity: 'info',
        title: 'System Operating Normally',
        subtitle: 'All fault thresholds clear. DC link stable, IGBT within limits, grid voltage nominal.',
        rootCause: [`Vdc held at ${vdcLast.toFixed(1)}V`, `IGBT peak = ${Math.max(...data.map(d=>d.igbtTemperature||0)).toFixed(1)}°C`, `Grid voltage nominal`],
        fixes: [],
      });
    }

    return results;
  }, [data]);

  // ── KPIs ─────────────────────────────────────────────────────────────────────
  const kpis = useMemo(() => {
    if (data.length === 0) return [];
    const vdcLast = data[data.length-1].dcLinkVoltage;
    const vdcMin  = Math.min(...data.map(d=>d.dcLinkVoltage));
    const maxIgbt = Math.max(...data.map(d=>d.igbtTemperature||0));
    const solarPk = Math.max(...data.map(d=>d.solarPowerWatts));
    const windPk  = Math.max(...data.map(d=>d.windPowerWatts||0));
    const socLast = data[data.length-1].batterySOC ?? 80;
    return [
      { label: 'DC Link (final)', value: vdcLast.toFixed(1), unit: 'V',  status: vdcLast<400?'danger':vdcLast<680?'warn':'ok' as any, sub: `Min: ${vdcMin.toFixed(1)}V` },
      { label: 'IGBT Tj Peak',    value: maxIgbt.toFixed(1), unit: '°C', status: maxIgbt>125?'danger':maxIgbt>100?'warn':'ok' as any, sub: 'Limit: 125°C' },
      { label: 'Solar Output',    value: (solarPk/1000).toFixed(1), unit: 'kW', status: 'ok' as any, sub: `${data[0].solarIrradiance}W/m²` },
      { label: 'Wind Output',     value: (windPk/1000).toFixed(1), unit: 'kW', status: windPk>0?'ok':'warn' as any, sub: windPk>0?'Connected':'No wind node' },
      { label: 'Battery SOC',     value: socLast.toFixed(1), unit: '%', status: socLast<20?'danger':socLast<50?'warn':'ok' as any, sub: 'End of simulation' },
      { label: 'Grid Status',     value: Math.min(...data.map(d=>Math.abs(d.gridVoltageA)))<150?'SAG':'NOMINAL', unit: '', status: Math.min(...data.map(d=>Math.abs(d.gridVoltageA)))<150?'danger':'ok' as any, sub: 'V_grid peak' },
    ];
  }, [data]);

  // ── Chart renderers ───────────────────────────────────────────────────────────
  const renderArea = (title: string, dataKey: string, color: string, unit: string, refLines?: {y:number; label:string; color:string}[]) => (
    <div className={`bg-white/80 backdrop-blur-sm border border-slate-200/60 rounded-xl p-5 shadow-sm hover:shadow-md transition-shadow flex flex-col h-64`}>
      <div className="text-[11px] font-black text-slate-400 uppercase tracking-widest mb-3 flex items-center gap-2">
        <div className="w-2 h-2 rounded-full" style={{ backgroundColor: color }} />
        {title}
      </div>
      <div className="flex-1 w-full">
        <ResponsiveContainer width="100%" height="100%">
          <AreaChart data={data} margin={{ top: 5, right: 5, left: -20, bottom: 0 }}>
            <defs>
              <linearGradient id={`grad-${dataKey}`} x1="0" y1="0" x2="0" y2="1">
                <stop offset="5%"  stopColor={color} stopOpacity={0.2} />
                <stop offset="95%" stopColor={color} stopOpacity={0.0} />
              </linearGradient>
            </defs>
            <CartesianGrid strokeDasharray="3 3" stroke="#f1f5f9" vertical={false} />
            <XAxis dataKey="time" tickFormatter={(t) => `${(t*1000).toFixed(0)}`} stroke="#cbd5e1" style={{fontSize:10,fontFamily:'monospace'}} tickLine={false} axisLine={false} />
            <YAxis stroke="#cbd5e1" style={{fontSize:10,fontFamily:'monospace'}} domain={['auto','auto']} unit={` ${unit}`} tickLine={false} axisLine={false} />
            <Tooltip
              contentStyle={{backgroundColor:'rgba(255,255,255,0.97)',borderColor:'#e2e8f0',borderRadius:'8px',boxShadow:'0 4px 6px -1px rgb(0 0 0/0.1)'}}
              labelStyle={{fontSize:11,color:'#64748b',fontFamily:'monospace',fontWeight:'bold'}}
              itemStyle={{fontSize:12,fontFamily:'monospace',fontWeight:'bold'}}
              labelFormatter={(l) => `Time: ${(l*1000).toFixed(2)} ms`}
            />
            {(refLines||[]).map(r => (
              <ReferenceLine key={r.y} y={r.y} stroke={r.color} strokeDasharray="4 2" label={{value:r.label,fill:r.color,fontSize:9}} />
            ))}
            <Area type="monotone" dataKey={dataKey} stroke={color} strokeWidth={2.5} fillOpacity={1} fill={`url(#grad-${dataKey})`} isAnimationActive={false} />
          </AreaChart>
        </ResponsiveContainer>
      </div>
    </div>
  );

  const renderLines = (title: string, keys: string[], colors: string[], unit: string) => (
    <div className="bg-white/80 backdrop-blur-sm border border-slate-200/60 rounded-xl p-5 shadow-sm hover:shadow-md transition-shadow flex flex-col h-64">
      <div className="text-[11px] font-black text-slate-400 uppercase tracking-widest mb-3 flex items-center gap-2">
        <Activity className="w-3.5 h-3.5 text-slate-400" />
        {title}
      </div>
      <div className="flex-1 w-full">
        <ResponsiveContainer width="100%" height="100%">
          <LineChart data={data} margin={{ top: 5, right: 5, left: -20, bottom: 0 }}>
            <CartesianGrid strokeDasharray="3 3" stroke="#f1f5f9" vertical={false} />
            <XAxis dataKey="time" tickFormatter={(t) => `${(t*1000).toFixed(0)}`} stroke="#cbd5e1" style={{fontSize:10,fontFamily:'monospace'}} tickLine={false} axisLine={false} />
            <YAxis stroke="#cbd5e1" style={{fontSize:10,fontFamily:'monospace'}} domain={['auto','auto']} unit={` ${unit}`} tickLine={false} axisLine={false} />
            <Tooltip
              contentStyle={{backgroundColor:'rgba(255,255,255,0.97)',borderColor:'#e2e8f0',borderRadius:'8px',boxShadow:'0 4px 6px -1px rgb(0 0 0/0.1)'}}
              labelStyle={{fontSize:11,color:'#64748b',fontFamily:'monospace',fontWeight:'bold'}}
              itemStyle={{fontSize:12,fontFamily:'monospace',fontWeight:'bold'}}
              labelFormatter={(l) => `Time: ${(l*1000).toFixed(2)} ms`}
            />
            {keys.map((k, i) => <Line key={k} type="monotone" dataKey={k} stroke={colors[i]} strokeWidth={2} dot={false} isAnimationActive={false} />)}
          </LineChart>
        </ResponsiveContainer>
      </div>
    </div>
  );

  if (isLoading) return <div className="min-h-screen bg-slate-50 flex items-center justify-center"><div className="animate-pulse text-sky-600 font-mono">Loading telemetry...</div></div>;

  if (data.length === 0) return (
    <div className="min-h-screen bg-slate-50 flex flex-col items-center justify-center font-mono">
      <Activity className="w-12 h-12 text-slate-300 mb-4" />
      <h2 className="text-xl font-bold text-slate-700">No Simulation Data Found</h2>
      <p className="text-slate-500 mb-6">Please run a simulation in the Workspace first.</p>
      <Link href="/" className="px-4 py-2 bg-sky-600 hover:bg-sky-500 text-white rounded font-bold transition-colors shadow-md">Go to Workspace</Link>
    </div>
  );

  return (
    <div className="min-h-screen bg-slate-50 text-slate-800 p-4 md:p-8 font-sans select-none flex flex-col relative overflow-hidden">
      <div className="absolute top-0 left-0 w-full h-[400px] bg-gradient-to-b from-slate-200/50 to-transparent pointer-events-none -z-10" />

      {/* Header */}
      <div className="flex flex-col md:flex-row md:items-center justify-between gap-4 mb-8">
        <div>
          <Link href="/" className="inline-flex items-center text-sky-600 hover:text-sky-700 mb-2 transition-colors text-[10px] font-black uppercase tracking-widest bg-sky-100/50 px-2 py-1 rounded">
            <ArrowLeft className="w-3 h-3 mr-1" /> Back to Workspace
          </Link>
          <h1 className="text-3xl font-extrabold text-slate-900 tracking-tight flex items-center gap-3 mt-1">
            <Activity className="w-8 h-8 text-sky-500" /> Diagnostics Dashboard
          </h1>
          <p className="text-slate-500 text-sm mt-1 max-w-xl leading-relaxed">High-fidelity EMT simulation results with automated fault analysis and physics-backed fix recommendations.</p>
        </div>
        <div className="flex items-center gap-3">
          <button onClick={handleDownloadFullRaw} className="flex items-center bg-white border border-slate-200 hover:border-sky-300 hover:text-sky-600 text-slate-600 px-5 py-2.5 rounded-lg shadow-sm transition-all font-bold text-xs">
            <Download className="w-4 h-4 mr-2" /> Full Raw JSON
          </button>
          <button onClick={handleExportCSV} className="flex items-center bg-sky-600 hover:bg-sky-700 text-white px-5 py-2.5 rounded-lg shadow-md hover:shadow-lg transition-all font-bold text-xs">
            <Download className="w-4 h-4 mr-2" /> Export CSV
          </button>
        </div>
      </div>

      {/* KPI Grid */}
      <div className="grid grid-cols-2 md:grid-cols-3 lg:grid-cols-6 gap-3 mb-8">
        {kpis.map((k,i) => <KpiCard key={i} {...k} />)}
      </div>

      {/* Diagnostic Cards */}
      <div className="mb-8 space-y-3">
        {diags.map(d => <DiagnosticCard key={d.id} diag={d} />)}
      </div>

      {/* Chart Zones */}
      <div className="mb-10">
        <h2 className="text-sm font-black text-slate-800 uppercase tracking-widest mb-4 flex items-center gap-2">
          <div className="w-1 h-4 bg-sky-500 rounded" /> Grid & Load Interface
        </h2>
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-4">
          {renderLines("Grid Voltage (3-Phase)", ["gridVoltageA","gridVoltageB","gridVoltageC"], ["#e11d48","#d97706","#0284c7"], "V")}
          {renderLines("Load Current (3-Phase)", ["loadCurrentA","loadCurrentB","loadCurrentC"], ["#e11d48","#d97706","#0284c7"], "A")}
        </div>
      </div>

      <div className="mb-10">
        <h2 className="text-sm font-black text-slate-800 uppercase tracking-widest mb-4 flex items-center gap-2">
          <div className="w-1 h-4 bg-purple-500 rounded" /> UPQC Power Electronics
        </h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
          {renderLines("Series Injection Voltage", ["injectingVoltageA","injectingVoltageB","injectingVoltageC"], ["#9333ea","#c084fc","#e879f9"], "V")}
          {renderLines("Shunt Injection Current", ["injectingCurrentA","injectingCurrentB","injectingCurrentC"], ["#9333ea","#c084fc","#e879f9"], "A")}
          {renderArea("DC Link Voltage", "dcLinkVoltage", "#f97316", "V", [
            { y: 700, label: 'Nominal 700V', color: '#16a34a' },
            { y: 400, label: 'FAULT <400V',  color: '#e11d48' },
          ])}
        </div>
      </div>

      <div className="mb-10">
        <h2 className="text-sm font-black text-slate-800 uppercase tracking-widest mb-4 flex items-center gap-2">
          <div className="w-1 h-4 bg-amber-500 rounded" /> Solar PV & Thermal Health
        </h2>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-4 gap-4">
          {renderArea("Solar PV Output", "solarPowerWatts", "#fbbf24", "W")}
          {renderArea("Solar DC Voltage (Vmpp)", "solarVoltageDc", "#eab308", "V")}
          {renderArea("Solar Irradiance", "solarIrradiance", "#f59e0b", "W/m²")}
          {renderArea("Panel Temperature", "solarTemperature", "#ef4444", "°C", [
            { y: 25, label: 'STC 25°C', color: '#16a34a' },
            { y: 65, label: 'Hot day 65°C', color: '#f97316' },
          ])}
        </div>
        <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4 mt-4">
          {renderArea("IGBT Junction Temp", "igbtTemperature", "#ef4444", "°C", [
            { y: 125, label: 'LIMIT 125°C', color: '#e11d48' },
          ])}
          {renderArea("Battery SOC", "batterySOC", "#10b981", "%", [
            { y: 20, label: 'Low SOC 20%', color: '#f97316' },
          ])}
          {renderArea("Wind Power", "windPowerWatts", "#06b6d4", "W")}
        </div>
      </div>

      {/* Raw Data Table */}
      <div className="bg-white border border-slate-200 rounded-xl shadow-sm flex flex-col overflow-hidden">
        <div className="px-5 py-4 border-b border-slate-100 bg-white flex items-center justify-between">
          <div className="text-sm font-bold text-slate-800 flex items-center gap-2">
            <ChevronRight className="w-4 h-4 text-sky-500" /> Raw Telemetry Table
          </div>
          <div className="text-[10px] font-black uppercase tracking-widest text-slate-400 bg-slate-100 px-2 py-1 rounded">{data.length} Rows</div>
        </div>
        <div className="w-full overflow-x-auto max-h-[400px]">
          <table className="w-full text-left text-[11px] font-mono text-slate-600 border-collapse whitespace-nowrap">
            <thead className="bg-slate-50 sticky top-0 shadow-sm z-10 border-b border-slate-200">
              <tr>
                {['Time (s)','Grid V (A)','Grid V (B)','Grid V (C)','Grid I (A)','Grid I (B)','Grid I (C)',
                  'Load V (A)','Load I (A)','Inj V (A)','Inj I (A)',
                  'DC Link (V)','Irrad (W/m²)','Temp (°C)','Vpv (V)','Ipv (A)','Ppv (W)','IGBT Tj (°C)','Wind (W)','SOC (%)']
                  .map(h => <th key={h} className="p-3 font-semibold text-slate-700">{h}</th>)}
              </tr>
            </thead>
            <tbody>
              {data.map((dp, i) => {
                const dcWarn = dp.dcLinkVoltage < 400 ? 'bg-rose-50' : dp.dcLinkVoltage < 680 ? 'bg-amber-50' : '';
                return (
                  <tr key={i} className={`hover:bg-sky-50 border-b border-slate-100 transition-colors ${dcWarn}`}>
                    <td className="p-3 font-bold text-slate-900 bg-slate-50/50">{dp.time.toFixed(4)}</td>
                    <td className="p-3">{dp.gridVoltageA.toFixed(1)}</td>
                    <td className="p-3">{dp.gridVoltageB.toFixed(1)}</td>
                    <td className="p-3">{dp.gridVoltageC.toFixed(1)}</td>
                    <td className="p-3">{dp.gridCurrentA.toFixed(1)}</td>
                    <td className="p-3">{dp.gridCurrentB.toFixed(1)}</td>
                    <td className="p-3">{dp.gridCurrentC.toFixed(1)}</td>
                    <td className="p-3">{dp.supplyVoltageA.toFixed(1)}</td>
                    <td className="p-3">{dp.loadCurrentA.toFixed(1)}</td>
                    <td className="p-3">{dp.injectingVoltageA.toFixed(1)}</td>
                    <td className="p-3">{dp.injectingCurrentA.toFixed(1)}</td>
                    <td className={`p-3 font-bold ${dp.dcLinkVoltage<400?'text-rose-600':dp.dcLinkVoltage<680?'text-amber-600':'text-orange-600'}`}>{dp.dcLinkVoltage.toFixed(1)}</td>
                    <td className="p-3 font-bold text-yellow-600">{dp.solarIrradiance.toFixed(0)}</td>
                    <td className="p-3 font-bold text-rose-500">{dp.solarTemperature?.toFixed(1) || '25.0'}</td>
                    <td className="p-3 font-bold text-amber-600">{dp.solarVoltageDc?.toFixed(1) || '0.0'}</td>
                    <td className="p-3 font-bold text-amber-600">{dp.solarCurrentDc?.toFixed(2) || '0.00'}</td>
                    <td className="p-3 font-bold text-amber-600">{dp.solarPowerWatts.toFixed(0)}</td>
                    <td className={`p-3 font-bold ${(dp.igbtTemperature||0)>125?'text-rose-600':'text-red-500'}`}>{dp.igbtTemperature?.toFixed(1) || '25.0'}</td>
                    <td className="p-3 font-bold text-cyan-600">{(dp.windPowerWatts||0).toFixed(0)}</td>
                    <td className="p-3 font-bold text-emerald-600">{(dp.batterySOC||0).toFixed(2)}</td>
                  </tr>
                );
              })}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
}
