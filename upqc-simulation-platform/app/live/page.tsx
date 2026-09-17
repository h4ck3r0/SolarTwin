"use client";

import React, { useEffect, useState } from "react";
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  ResponsiveContainer,
  Tooltip,
} from "recharts";
import { Activity, Sun, Thermometer, Zap, ActivitySquare, Download, AlertTriangle, Cpu } from "lucide-react";

export default function LivePredictionDashboard() {
  const [history, setHistory] = useState<any[]>([]);
  const [current, setCurrent] = useState<any>(null);
  const [historyLimit, setHistoryLimit] = useState<number>(60);

  useEffect(() => {
    const fetchLiveData = async () => {
      try {
        const res = await fetch("http://localhost:8000/api/live_data");
        const data = await res.json();
        
        const processData = (point: any) => {
          if (!point) return point;
          const newPoint = { ...point };
          for (const key in newPoint) {
            if (key.includes("THD")) newPoint[key] = newPoint[key] * 100;
          }
          return newPoint;
        };

        if (data.history) setHistory(data.history.map(processData));
        if (data.current) setCurrent(processData(data.current));
      } catch (err) {
        console.error("Failed to fetch live data", err);
      }
    };

    const interval = setInterval(fetchLiveData, 1000);
    return () => clearInterval(interval);
  }, []);

  const handleExportCSV = () => {
    if (history.length === 0) return;
    const headers = Object.keys(history[0]);
    const rows = history.map(dp => headers.map(k => dp[k]));
    const csvContent = 'data:text/csv;charset=utf-8,' + [headers.join(','), ...rows.map(e => e.join(','))].join('\\n');
    const link = document.createElement('a');
    link.setAttribute('href', encodeURI(csvContent));
    link.setAttribute('download', 'live_history.csv');
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  // AI Predictive Health Logic
  const isVdcDropping = current && current.Vdc < 650;
  const isGridSag = current && current.Vrms_1 < 220;
  const isHighTHD = current && current.THDv_1 > 5;
  const showAiWarning = isVdcDropping || isGridSag || isHighTHD;

  const ChartMonitor = ({
    title,
    dataKey,
    color,
    icon: Icon,
    unit,
    isWarning = false,
  }: {
    title: string;
    dataKey: string;
    color: string;
    icon: any;
    unit: string;
    isWarning?: boolean;
  }) => {
    const currentValue = current ? current[dataKey] : 0;
    
    return (
      <div className={`flex flex-col bg-white/90 backdrop-blur-md border ${isWarning ? 'border-rose-300 ring-2 ring-rose-100' : 'border-slate-200'} rounded-2xl overflow-hidden shadow-sm hover:shadow-md transition-all`}>
        <div className="flex items-center justify-between p-5 border-b border-slate-100">
          <div className="flex items-center gap-3">
            <div className={`p-2 rounded-lg ${isWarning ? 'bg-rose-100 text-rose-600' : 'bg-slate-100 text-slate-500'}`}>
              <Icon className="w-5 h-5" />
            </div>
            <h3 className="font-bold text-slate-700 tracking-tight text-sm">{title}</h3>
          </div>
          <div className="flex items-baseline gap-1.5">
            <span className={`text-2xl font-black tracking-tight ${isWarning ? 'text-rose-600' : 'text-slate-800'}`}>
              {currentValue ? currentValue.toFixed(2) : "0.00"}
            </span>
            <span className="text-slate-400 font-bold text-xs">{unit}</span>
          </div>
        </div>

        <div className="h-[200px] w-full p-4">
          <ResponsiveContainer width="100%" height="100%">
            <LineChart data={history.slice(-historyLimit)}>
              <CartesianGrid strokeDasharray="3 3" stroke="#f1f5f9" vertical={false} />
              <XAxis dataKey="Temp" hide />
              <Tooltip 
                contentStyle={{ backgroundColor: "rgba(255,255,255,0.95)", backdropFilter: "blur(4px)", border: "1px solid #e2e8f0", borderRadius: "12px", boxShadow: "0 10px 15px -3px rgb(0 0 0 / 0.1)" }} 
                itemStyle={{ color: isWarning ? '#e11d48' : color, fontWeight: 'bold' }} 
              />
              <YAxis domain={['auto', 'auto']} hide />
              <Line
                type="monotone"
                dataKey={dataKey}
                stroke={isWarning ? '#e11d48' : color}
                strokeWidth={3.5}
                dot={false}
                isAnimationActive={false}
                style={{ filter: `drop-shadow(0px 4px 6px ${isWarning ? '#e11d4840' : color + '40'})` }}
              />
            </LineChart>
          </ResponsiveContainer>
        </div>
      </div>
    );
  };

  return (
    <div className="min-h-full bg-slate-50 text-slate-900 p-6 md:p-10 font-sans relative overflow-hidden">
      {/* Background ambient gradient */}
      <div className="absolute top-0 left-0 w-full h-[500px] bg-gradient-to-b from-sky-100/40 to-transparent pointer-events-none -z-10"></div>
      
      <div className="max-w-7xl mx-auto space-y-8">
        
        {/* Header section */}
        <header className="flex flex-col xl:flex-row justify-between items-start xl:items-center gap-6 pb-8 border-b border-slate-200/60">
          <div>
            <h1 className="text-3xl font-extrabold flex items-center gap-3 tracking-tight text-slate-900">
              <Activity className="w-8 h-8 text-sky-500" />
              Live Forecasting Matrix
            </h1>
            <p className="text-slate-500 mt-2 text-sm max-w-2xl leading-relaxed">
              Real-time inference using the trained PyTorch LSTM model. Predicting grid states based on live weather data from RNSIT College, Bangalore.
            </p>
          </div>
          
          <div className="flex flex-wrap items-center gap-4">
            <select 
              value={historyLimit} 
              onChange={(e) => setHistoryLimit(Number(e.target.value))}
              className="bg-white border border-slate-200 text-slate-700 text-xs font-bold rounded-lg px-4 py-2.5 shadow-sm focus:outline-none focus:ring-2 focus:ring-sky-500"
            >
              <option value={60}>Last 1 Minute</option>
              <option value={300}>Last 5 Minutes</option>
              <option value={900}>Last 15 Minutes</option>
              <option value={3600}>Last 1 Hour</option>
            </select>
            <button 
              onClick={handleExportCSV}
              className="flex items-center gap-2 bg-sky-600 hover:bg-sky-700 text-white px-5 py-2.5 rounded-lg text-xs font-bold transition-all shadow-md hover:shadow-lg"
            >
              <Download className="w-4 h-4" />
              Export Model Telemetry
            </button>
          </div>
        </header>

        {/* AI Health Banner */}
        {showAiWarning && (
          <div className="bg-gradient-to-r from-rose-50 to-orange-50 border border-rose-200/60 p-6 rounded-2xl shadow-sm flex items-start gap-4 animate-in fade-in slide-in-from-top-4">
            <div className="bg-rose-100 p-3 rounded-full shrink-0">
              <Cpu className="w-6 h-6 text-rose-600" />
            </div>
            <div>
              <h2 className="text-rose-900 font-extrabold text-lg tracking-tight flex items-center gap-2">
                <AlertTriangle className="w-5 h-5 text-rose-600" />
                AI Predictive Anomaly Detected
              </h2>
              <p className="text-rose-700/80 text-sm mt-1 max-w-3xl leading-relaxed">
                The LSTM neural network is currently predicting an imminent system fault based on the latest sequence buffer. 
                {isVdcDropping && " DC Link voltage collapse predicted."}
                {isGridSag && " Severe microgrid voltage sag predicted."}
                {isHighTHD && " Harmonic distortion exceeding IEEE 519 compliance predicted."}
              </p>
            </div>
          </div>
        )}

        {/* Weather Feed Row */}
        <div className="flex gap-4">
          <div className="bg-white/80 backdrop-blur-md border border-slate-200/60 shadow-sm p-4 rounded-xl flex items-center gap-6 px-6">
            <div className="flex flex-col">
              <span className="text-[10px] font-black text-slate-400 uppercase tracking-widest mb-1">Live Irradiance</span>
              <div className="flex items-center gap-2">
                <Sun className="w-5 h-5 text-amber-500" />
                <span className="font-black text-2xl text-slate-800 tracking-tight">
                  {current?.Irradiance?.toFixed(0) || "---"} <span className="text-sm text-slate-400 font-bold">W/m²</span>
                </span>
              </div>
            </div>
            <div className="w-px h-10 bg-slate-200"></div>
            <div className="flex flex-col">
              <span className="text-[10px] font-black text-slate-400 uppercase tracking-widest mb-1">Ambient Temp</span>
              <div className="flex items-center gap-2">
                <Thermometer className="w-5 h-5 text-orange-500" />
                <span className="font-black text-2xl text-slate-800 tracking-tight">
                  {current?.Temp?.toFixed(1) || "--"} <span className="text-sm text-slate-400 font-bold">°C</span>
                </span>
              </div>
            </div>
          </div>
        </div>

        {/* Primary Monitors */}
        <div className="grid grid-cols-1 lg:grid-cols-2 gap-6">
          <ChartMonitor 
            title="Total Active Power (Pac)" 
            dataKey="Pac" 
            color="#0ea5e9"
            icon={Zap}
            unit="W"
          />
          <ChartMonitor 
            title="Microgrid Voltage (Vrms)" 
            dataKey="Vrms_1" 
            color="#10b981"
            icon={ActivitySquare}
            unit="V"
            isWarning={isGridSag}
          />
        </div>

        {/* Secondary Grid */}
        <div className="grid grid-cols-1 md:grid-cols-3 gap-6">
          <ChartMonitor 
            title="Microgrid Current (Irms)" 
            dataKey="Irms_1" 
            color="#f59e0b"
            icon={Activity}
            unit="A"
          />
          <ChartMonitor 
            title="DC Link Voltage (Vdc)" 
            dataKey="Vdc" 
            color="#8b5cf6"
            icon={Zap}
            unit="V"
            isWarning={isVdcDropping}
          />
          <ChartMonitor 
            title="Voltage THD" 
            dataKey="THDv_1" 
            color="#64748b"
            icon={ActivitySquare}
            unit="%"
            isWarning={isHighTHD}
          />
        </div>

      </div>
    </div>
  );
}
