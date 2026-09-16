'use client';

import React, { useState } from 'react';
import { Copy, Check } from 'lucide-react';
import { SimulationParameters } from '@/lib/simulation-types';

interface SolarPVTelemetryCardProps {
  parameters: SimulationParameters;
  topology: { nodes: any[]; edges: any[] } | null;
  onUpdateParameters: (newParams: SimulationParameters) => void;
  onUpdateNode?: (nodeId: string, params: any) => void;
  isSimulating?: boolean;
}

export default function SolarPVTelemetryCard({
  parameters,
  topology,
  onUpdateParameters,
  onUpdateNode,
  isSimulating = false,
}: SolarPVTelemetryCardProps) {
  const [copied, setCopied] = useState(false);

  // Aggregate from topology nodes
  const solarNodes = topology?.nodes?.filter(n => n.type === 'microgrid' && (n.data?.label?.toLowerCase().includes('solar') || n.data?.label?.toLowerCase().includes('pv'))) || [];
  
  let totalCapacityWatts = 0;
  let currentPowerWatts = 0;
  let totalStrings = 0;
  let avgModules = 0;
  let avgIrradiance = 0;

  if (solarNodes.length > 0) {
    solarNodes.forEach(node => {
      const p = node.data?.parameters || {};
      const strings = p.solarStringsParallel ?? 88;
      const mods = p.solarModulesSeries ?? 7;
      const rating = p.solarPanelWatts ?? 415;
      const irr = p.solarIrradiance ?? 1000;
      
      const cap = strings * mods * rating;
      totalCapacityWatts += cap;
      currentPowerWatts += cap * Math.max(0, irr / 1000);
      totalStrings += strings;
      avgModules += mods;
      avgIrradiance += irr;
    });
    avgModules = Math.round(avgModules / solarNodes.length);
    avgIrradiance = Math.round(avgIrradiance / solarNodes.length);
  } else {
    // Fallback to global if no nodes
    const strings = parameters.solarStringsParallel ?? 88;
    const mods = parameters.solarModulesSeries ?? 7;
    const rating = parameters.solarPanelWatts ?? 415;
    totalCapacityWatts = strings * mods * rating;
    avgIrradiance = parameters.solarIrradiance ?? 1000;
    currentPowerWatts = totalCapacityWatts * Math.max(0, avgIrradiance / 1000);
    totalStrings = strings;
    avgModules = mods;
  }

  const currentPowerKw = (currentPowerWatts / 1000).toFixed(3);
  
  const voltageDc = currentPowerWatts > 0 ? 510.3 : 0;
  const currentDc = voltageDc > 0 ? Math.round((currentPowerWatts / voltageDc) * 100) / 100 : 0;

  const textSummary = `P: ${currentPowerWatts.toFixed(1)}W • V: ${voltageDc}Vdc • I: ${currentDc}A • Irr: ${avgIrradiance}W/m²`;

  const handleCopyText = () => {
    navigator.clipboard.writeText(`${avgModules}S×${totalStrings}P Solar Array (${(totalCapacityWatts/1000).toFixed(3)} kW) producing ${voltageDc}V DC at ${currentDc}A = ${currentPowerWatts.toFixed(1)}W (${currentPowerKw} kW) active power.`);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const setPresetIrradiance = (value: number) => {
    if (onUpdateNode && solarNodes.length > 0) {
      solarNodes.forEach(n => {
        onUpdateNode(n.id, { ...n.data.parameters, solarIrradiance: value });
      });
    }
    onUpdateParameters({
      ...parameters,
      solarIrradiance: value,
    });
  };

  return (
    <div className="bg-white border border-slate-200 rounded px-2.5 py-1.5 font-mono select-none text-[10px] shadow-lg flex items-center justify-between gap-2 overflow-hidden">
      {/* 1. Left: 5 Compact Solar Modules Graphic */}
      <div className="flex items-center space-x-1.5 border-r border-slate-200 pr-2.5">
        <div className="flex items-center space-x-1">
          {[1, 2, 3, 4, 5, 6, 7].map((idx) => (
            <div key={idx} className="flex flex-col items-center">
              <div className={`w-5 h-7 rounded-[2px] border flex flex-col justify-between p-0.5 ${
                currentPowerWatts > 0 ? 'bg-amber-950/40 border-amber-500/60 shadow-sm shadow-amber-500/20' : 'bg-slate-100 border-slate-300'
              }`}>
                <div className="grid grid-cols-2 gap-[1px] h-full">
                  {[...Array(6)].map((_, cIdx) => (
                    <div key={cIdx} className={`${currentPowerWatts > 0 ? 'bg-amber-400' : 'bg-slate-800'} rounded-[0.5px]`} />
                  ))}
                </div>
              </div>
            </div>
          ))}
        </div>

        <div className="hidden xl:flex items-center space-x-1 text-slate-500 text-[8px] pl-1 font-sans">
          <span>──</span>
          <span className="font-mono text-slate-500 text-[7.5px]">Series String</span>
        </div>
      </div>

      {/* 2. Three Compact Metric Display Boxes */}
      <div className="flex items-center space-x-2">
        <div className="bg-white border border-slate-200 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-500 uppercase">TOTAL ACTIVE POWER</div>
          <div className="text-xs font-bold text-amber-600 leading-tight">
            {currentPowerWatts} <span className="text-[9px] text-amber-600">W</span>
          </div>
          <div className="text-[7px] text-slate-500">{currentPowerKw} kW</div>
        </div>

        <div className="bg-white border border-slate-200 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-500 uppercase">STRING VOLTAGE</div>
          <div className="text-xs font-bold text-sky-600 leading-tight">
            {voltageDc} <span className="text-[9px] text-sky-600">V DC</span>
          </div>
          <div className="text-[7px] text-slate-500">{avgModules} panels series</div>
        </div>

        <div className="bg-white border border-slate-200 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-500 uppercase">ARRAY CURRENT</div>
          <div className="text-xs font-bold text-emerald-600 leading-tight">
            {currentDc.toFixed(2)} <span className="text-[9px] text-emerald-600">A DC</span>
          </div>
          <div className="text-[7px] text-slate-500">{totalStrings} strings parallel</div>
        </div>
      </div>

      {/* 3. Compact Horizontal Irradiance Selector */}
      <div className="flex items-center space-x-1 border-l border-r border-slate-200 px-2">
        <span className="text-[7.5px] font-bold text-sky-600 uppercase mr-0.5">IRR</span>
        {[
          { label: '1000 W/m²', val: 1000 },
          { label: '800 W/m²', val: 800 },
          { label: '500 W/m²', val: 500 },
          { label: '200 W/m²', val: 200 },
          { label: '0 W/m²', val: 0 },
        ].map((item) => (
          <button
            key={item.val}
            type="button"
            onClick={() => setPresetIrradiance(item.val)}
            className={`px-1.5 py-0.5 rounded text-[8px] font-mono transition-colors border ${
              avgIrradiance === item.val
                ? 'bg-amber-500/20 text-amber-600 font-bold border-amber-500/60'
                : 'bg-white text-slate-500 border-slate-200 hover:text-slate-700'
            }`}
          >
            {item.val}
          </button>
        ))}
      </div>

      {/* 4. One-Line Telemetry Callout */}
      <div className="flex items-center space-x-1.5 bg-white border border-slate-200 px-2 py-1 rounded max-w-[190px]">
        <div className="text-[7.5px] text-emerald-600 font-mono leading-tight truncate">
          {textSummary}
        </div>
        <button
          type="button"
          onClick={handleCopyText}
          className="flex-shrink-0 px-1 py-0.5 rounded bg-slate-100 border border-slate-700 text-slate-700 text-[7.5px] hover:text-amber-600"
          title="Copy Telemetry Text"
        >
          {copied ? <Check className="w-2.5 h-2.5 text-emerald-600" /> : <Copy className="w-2.5 h-2.5" />}
        </button>
      </div>
    </div>
  );
}
