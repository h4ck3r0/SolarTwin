'use client';

import React, { useState } from 'react';
import { Copy, Check } from 'lucide-react';
import { SimulationParameters } from '@/lib/simulation-types';

interface SolarPVTelemetryCardProps {
  parameters: SimulationParameters;
  onUpdateParameters: (newParams: SimulationParameters) => void;
  isSimulating?: boolean;
}

export default function SolarPVTelemetryCard({
  parameters,
  onUpdateParameters,
  isSimulating = false,
}: SolarPVTelemetryCardProps) {
  const [copied, setCopied] = useState(false);

  const count = parameters.solarPanelCount ?? 5;
  const wattsPerPanel = parameters.solarPanelWatts ?? 305;
  const totalCapacityWatts = count * wattsPerPanel; // 1525 W
  const irradiance = parameters.solarIrradiance; // e.g. 1000 W/m²

  const irradianceFactor = Math.max(0, irradiance / 1000);
  const currentPowerWatts = Math.round(totalCapacityWatts * irradianceFactor * 10) / 10;
  const currentPowerKw = (currentPowerWatts / 1000).toFixed(3);
  
  const voltageDc = currentPowerWatts > 0 ? 163.5 : 0;
  const currentDc = voltageDc > 0 ? Math.round((currentPowerWatts / voltageDc) * 100) / 100 : 0;
  const perPanelPower = currentPowerWatts > 0 ? Math.round((currentPowerWatts / count) * 10) / 10 : 0;

  const textSummary = `P: ${currentPowerWatts}W • V: ${voltageDc}Vdc • I: ${currentDc}A • Irr: ${irradiance}W/m² • IDLE 0.0000s`;

  const handleCopyText = () => {
    navigator.clipboard.writeText(`5x 305W Solar Panels (1.525 kW) producing ${voltageDc}V DC at ${currentDc}A = ${currentPowerWatts}W (${currentPowerKw} kW) active power.`);
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  const setPresetIrradiance = (value: number) => {
    onUpdateParameters({
      ...parameters,
      solarIrradiance: value,
    });
  };

  return (
    <div className="bg-[#0b101b] border border-cyan-950/80 rounded px-2.5 py-1.5 font-mono select-none text-[10px] shadow-lg flex items-center justify-between gap-2 overflow-hidden">
      {/* 1. Left: 5 Compact Solar Modules Graphic */}
      <div className="flex items-center space-x-1.5 border-r border-slate-850 pr-2.5">
        <div className="flex items-center space-x-1">
          {[1, 2, 3, 4, 5].map((idx) => (
            <div key={idx} className="flex flex-col items-center">
              <div className={`w-7 h-7 rounded-[2px] border flex flex-col justify-between p-0.5 ${
                currentPowerWatts > 0 ? 'bg-amber-950/40 border-amber-500/60 shadow-sm shadow-amber-500/20' : 'bg-slate-950 border-slate-800'
              }`}>
                <div className="grid grid-cols-2 gap-[1px] h-full">
                  {[...Array(6)].map((_, cIdx) => (
                    <div key={cIdx} className={`${currentPowerWatts > 0 ? 'bg-amber-400' : 'bg-slate-800'} rounded-[0.5px]`} />
                  ))}
                </div>
              </div>
              <div className="text-[7px] font-bold text-amber-400 mt-0.5 leading-none">PV #{idx}</div>
              <div className="text-[6.5px] text-amber-300/80 leading-none">{perPanelPower}W</div>
            </div>
          ))}
        </div>

        <div className="hidden xl:flex items-center space-x-1 text-slate-500 text-[8px] pl-1 font-sans">
          <span>──</span>
          <span className="font-mono text-slate-400 text-[7.5px]">Series String</span>
        </div>
      </div>

      {/* 2. Three Compact Metric Display Boxes */}
      <div className="flex items-center space-x-2">
        <div className="bg-[#070b14] border border-slate-850 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-400 uppercase">TOTAL ACTIVE POWER</div>
          <div className="text-xs font-bold text-amber-400 leading-tight">
            {currentPowerWatts} <span className="text-[9px] text-amber-300">W</span>
          </div>
          <div className="text-[7px] text-slate-500">{currentPowerKw} kW</div>
        </div>

        <div className="bg-[#070b14] border border-slate-850 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-400 uppercase">STRING VOLTAGE</div>
          <div className="text-xs font-bold text-cyan-400 leading-tight">
            {voltageDc} <span className="text-[9px] text-cyan-300">V DC</span>
          </div>
          <div className="text-[7px] text-slate-500">5 panels series</div>
        </div>

        <div className="bg-[#070b14] border border-slate-850 px-2 py-0.5 rounded min-w-[95px]">
          <div className="text-[7.5px] font-bold text-slate-400 uppercase">ARRAY CURRENT</div>
          <div className="text-xs font-bold text-emerald-400 leading-tight">
            {currentDc.toFixed(2)} <span className="text-[9px] text-emerald-300">A DC</span>
          </div>
          <div className="text-[7px] text-slate-500">Isc STC: 9.33A</div>
        </div>
      </div>

      {/* 3. Compact Horizontal Irradiance Selector */}
      <div className="flex items-center space-x-1 border-l border-r border-slate-850 px-2">
        <span className="text-[7.5px] font-bold text-cyan-400 uppercase mr-0.5">IRR</span>
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
              irradiance === item.val
                ? 'bg-amber-500/20 text-amber-300 font-bold border-amber-500/60'
                : 'bg-[#070b14] text-slate-400 border-slate-850 hover:text-slate-200'
            }`}
          >
            {item.val}
          </button>
        ))}
      </div>

      {/* 4. One-Line Telemetry Callout */}
      <div className="flex items-center space-x-1.5 bg-[#070b14] border border-slate-850 px-2 py-1 rounded max-w-[190px]">
        <div className="text-[7.5px] text-emerald-400 font-mono leading-tight truncate">
          {textSummary}
        </div>
        <button
          type="button"
          onClick={handleCopyText}
          className="flex-shrink-0 px-1 py-0.5 rounded bg-slate-900 border border-slate-700 text-slate-300 text-[7.5px] hover:text-amber-400"
          title="Copy Telemetry Text"
        >
          {copied ? <Check className="w-2.5 h-2.5 text-emerald-400" /> : <Copy className="w-2.5 h-2.5" />}
        </button>
      </div>
    </div>
  );
}
