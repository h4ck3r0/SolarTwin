import re

with open('upqc-simulation-platform/app/statistics/page.tsx', 'r') as f:
    content = f.read()

replacements = [
    (r'bg-\[\#050912\] text-slate-100', r'bg-slate-50 text-slate-900'),
    (r'text-cyan-400 hover:text-cyan-300', r'text-sky-600 hover:text-sky-500'),
    (r'font-bold text-white', r'font-bold text-slate-900'),
    (r'text-cyan-400', r'text-sky-600'),
    (r'text-slate-400', r'text-slate-500'),
    (r'bg-cyan-600 hover:bg-cyan-500 text-white px-5 py-2\.5 rounded shadow-lg shadow-cyan-900/50', r'bg-sky-600 hover:bg-sky-500 text-white px-5 py-2.5 rounded shadow-sm shadow-slate-300'),
    (r'bg-\[\#0a1122\] rounded-xl border border-amber-500/30 p-4 flex flex-col shadow-xl aspect-square', r'bg-white rounded-xl border border-slate-200 p-4 flex flex-col shadow-sm aspect-square'),
    (r'bg-\[\#0a1122\] rounded-xl border border-[a-z]+-500/30 p-4 flex flex-col shadow-xl aspect-square', r'bg-white rounded-xl border border-slate-200 p-4 flex flex-col shadow-sm aspect-square'),
    (r'text-amber-400', r'text-amber-600'),
    (r'bg-amber-500/20 text-amber-300 border border-amber-500/40', r'bg-amber-50 text-amber-700 border border-amber-200'),
    (r'stroke="#1a2b4c"', r'stroke="#e2e8f0"'),
    (r"backgroundColor: '#060b13'", r"backgroundColor: '#ffffff'"),
    (r'text-cyan-400', r'text-sky-700'),
    (r'bg-cyan-500/20 text-cyan-300 border border-cyan-500/40', r'bg-sky-50 text-sky-700 border border-sky-200'),
    (r'text-orange-400', r'text-orange-600'),
    (r'bg-orange-500/20 text-orange-300 border border-orange-500/40', r'bg-orange-50 text-orange-700 border border-orange-200'),
    (r'text-emerald-400', r'text-emerald-700'),
    (r'bg-emerald-500/20 text-emerald-300 border border-emerald-500/40', r'bg-emerald-50 text-emerald-700 border border-emerald-200'),
    (r'text-purple-400', r'text-purple-700'),
    (r'bg-purple-500/20 text-purple-300 border border-purple-500/40', r'bg-purple-50 text-purple-700 border border-purple-200'),
    (r'stroke="#00f0ff"', r'stroke="#0284c7"'), # Phase C
]

for old, new in replacements:
    content = re.sub(old, new, content)

with open('upqc-simulation-platform/app/statistics/page.tsx', 'w') as f:
    f.write(content)

