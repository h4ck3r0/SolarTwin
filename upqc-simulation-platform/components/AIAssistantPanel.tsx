import React, { useState } from 'react';
import { Bot, Loader2, X } from 'lucide-react';
import { SimulationParameters } from '@/lib/simulation-types';

export default function AIAssistantPanel({ parameters }: { parameters: SimulationParameters }) {
  const [isOpen, setIsOpen] = useState(false);
  const [loading, setLoading] = useState(false);
  const [advice, setAdvice] = useState<string | null>(null);

  const handleAskAI = async () => {
    setLoading(true);
    try {
      const response = await fetch('http://localhost:8001/api/agent/analyze', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ state: parameters }),
      });
      const data = await response.json();
      if (data.success) {
        setAdvice(data.advice);
      } else {
        setAdvice('Error: ' + data.advice);
      }
    } catch (e: any) {
      setAdvice('Failed to connect to AI Agent API: ' + e.message);
    } finally {
      setLoading(false);
    }
  };

  if (!isOpen) {
    return (
      <button 
        onClick={() => setIsOpen(true)}
        className="absolute bottom-4 right-80 z-[100] flex items-center gap-2 bg-indigo-600 hover:bg-indigo-500 text-white px-4 py-2 rounded-full shadow-xl transition-all"
      >
        <Bot className="w-5 h-5" />
        <span className="font-bold text-sm">Ask AI Operator</span>
      </button>
    );
  }

  return (
    <div className="absolute bottom-4 right-80 z-[100] w-80 bg-white border border-slate-200 rounded-xl shadow-2xl flex flex-col overflow-hidden animate-in fade-in slide-in-from-bottom-4">
      <div className="bg-indigo-600 text-white px-4 py-3 flex items-center justify-between">
        <div className="flex items-center gap-2">
          <Bot className="w-5 h-5" />
          <h3 className="font-bold text-sm">AI Microgrid Operator</h3>
        </div>
        <button onClick={() => setIsOpen(false)} className="hover:bg-indigo-500 p-1 rounded">
          <X className="w-4 h-4" />
        </button>
      </div>
      
      <div className="p-4 bg-slate-50 min-h-[120px] max-h-[300px] overflow-y-auto text-sm text-slate-700">
        {loading ? (
          <div className="flex flex-col items-center justify-center h-full text-indigo-600 gap-2 pt-6">
            <Loader2 className="w-6 h-6 animate-spin" />
            <span className="text-xs font-semibold mt-2">Analyzing telemetry...</span>
          </div>
        ) : advice ? (
          <div className="whitespace-pre-wrap leading-relaxed">{advice}</div>
        ) : (
          <p className="text-slate-500 italic text-center mt-6">Click below to ask the AI for optimization advice based on current telemetry.</p>
        )}
      </div>

      <div className="p-3 bg-white border-t border-slate-200">
        <button 
          onClick={handleAskAI}
          disabled={loading}
          className="w-full bg-indigo-600 hover:bg-indigo-700 disabled:bg-indigo-300 text-white font-bold py-2 rounded-lg text-sm transition-colors"
        >
          {loading ? 'Thinking...' : 'Analyze Current State'}
        </button>
      </div>
    </div>
  );
}
