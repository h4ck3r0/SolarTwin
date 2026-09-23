import React from 'react';
import { LogicState } from '../lib/logic-analyzer';
import { Zap, Sun, Battery, Activity, ArrowRight, ShieldAlert, Cpu } from 'lucide-react';

export default function PlainEnglishCards({ logic }: { logic: LogicState }) {
  const getBorderColor = (state: string) => {
    if (state.includes('Generating') || state.includes('Supplying') || state.includes('Active')) return 'border-emerald-500';
    if (state.includes('Charging') || state.includes('Exporting') || state.includes('Absorbing')) return 'border-sky-500';
    if (state.includes('Discharging') || state.includes('Sag') || state.includes('Swell')) return 'border-amber-500';
    return 'border-slate-300';
  };

  const getIconColor = (state: string) => {
    if (state.includes('Generating') || state.includes('Supplying') || state.includes('Active')) return 'text-emerald-500';
    if (state.includes('Charging') || state.includes('Exporting') || state.includes('Absorbing')) return 'text-sky-500';
    if (state.includes('Discharging') || state.includes('Sag') || state.includes('Swell')) return 'text-amber-500';
    return 'text-slate-400';
  };

  return (
    <div className="grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4">
      {/* Solar Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm ${getBorderColor(logic.solar.state)}`}>
        <div className="flex items-center gap-2 mb-2">
          <Sun className={`w-5 h-5 ${getIconColor(logic.solar.state)}`} />
          <h3 className="font-bold text-slate-800 text-sm">Solar Generation</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            {logic.solar.state}
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.solar.description}</p>
      </div>

      {/* Grid Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm ${getBorderColor(logic.grid.state)}`}>
        <div className="flex items-center gap-2 mb-2">
          <Zap className={`w-5 h-5 ${getIconColor(logic.grid.state)}`} />
          <h3 className="font-bold text-slate-800 text-sm">Main Grid</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            {logic.grid.state}
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.grid.description}</p>
      </div>

      {/* Battery Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm ${getBorderColor(logic.battery.state)}`}>
        <div className="flex items-center gap-2 mb-2">
          <Battery className={`w-5 h-5 ${getIconColor(logic.battery.state)}`} />
          <h3 className="font-bold text-slate-800 text-sm">Battery Storage</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            {logic.battery.state}
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.battery.description}</p>
      </div>

      {/* UPQC Series Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm ${getBorderColor(logic.upqcSeries.state)}`}>
        <div className="flex items-center gap-2 mb-2">
          <ShieldAlert className={`w-5 h-5 ${getIconColor(logic.upqcSeries.state)}`} />
          <h3 className="font-bold text-slate-800 text-sm">Voltage Protection (Series)</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            {logic.upqcSeries.state}
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.upqcSeries.description}</p>
      </div>

      {/* UPQC Shunt Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm ${getBorderColor(logic.upqcShunt.state)}`}>
        <div className="flex items-center gap-2 mb-2">
          <Activity className={`w-5 h-5 ${getIconColor(logic.upqcShunt.state)}`} />
          <h3 className="font-bold text-slate-800 text-sm">Current Cleaning (Shunt)</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            {logic.upqcShunt.state}
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.upqcShunt.description}</p>
      </div>
      
      {/* Load Logic */}
      <div className={`bg-white rounded p-4 border-l-4 shadow-sm border-purple-500`}>
        <div className="flex items-center gap-2 mb-2">
          <Cpu className={`w-5 h-5 text-purple-500`} />
          <h3 className="font-bold text-slate-800 text-sm">Load Demand</h3>
        </div>
        <div className="mb-2">
          <span className="text-xs font-bold uppercase px-2 py-1 bg-slate-100 rounded text-slate-600">
            Consuming
          </span>
        </div>
        <p className="text-sm text-slate-600">{logic.load.description}</p>
      </div>
    </div>
  );
}
