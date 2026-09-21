'use client';

import React, { useEffect, useMemo, useRef, useCallback, useState } from 'react';
import ReactFlow, {
  Background,
  NodeTypes,
  useNodesState,
  useEdgesState,
  Position,
  NodeProps,
  useReactFlow,
  Panel,
  Connection,
  Edge,
  addEdge,
  Handle
} from 'reactflow';
import 'reactflow/dist/style.css';
import { initialNodes, initialEdges, ElectricalNodeData } from '@/lib/model-definition';
import { Cpu, Eye, Radio, Sun, Zap } from 'lucide-react';

const CustomElectricalNode = ({ id, data, selected }: NodeProps<ElectricalNodeData>) => {
  const getNodeStyling = () => {
    if (id === 'grid-source') return 'border-sky-500 bg-white text-sky-800';
    if (id === 'series-trans') return 'border-sky-500 bg-white text-sky-700';
    if (id === 'series-inv') return 'border-emerald-500 bg-white text-emerald-800';
    if (id === 'shunt-inv') return 'border-purple-500 bg-white text-purple-800';
    if (id === 'dc-link') return 'border-amber-400 bg-white text-amber-600 font-bold';
    if (id === 'microgrid') return 'border-amber-400 bg-white text-amber-700';
    if (id === 'critical-load') return 'border-emerald-500 bg-white text-emerald-800';
    if (id === 'diode-rectifier') return 'border-rose-500 bg-white text-rose-800';
    if (id === 'scope-block') return 'border-sky-500 bg-white text-sky-700';
    if (id.includes('mppt')) return 'border-amber-500 bg-amber-50 text-amber-800 font-bold';
    if (id.includes('battery') || id.includes('bess')) return 'border-emerald-500 bg-emerald-50 text-emerald-800 font-bold';
    if (id.includes('wind') || id.includes('turbine')) return 'border-cyan-500 bg-cyan-50 text-cyan-800 font-bold';
    return 'border-slate-300 bg-white text-slate-700';
  };

  const renderIcon = () => {
    if (id === 'grid-source') return <Radio className="w-5 h-5 text-sky-600" />;
    if (id === 'series-trans') return <span className="text-sm font-bold text-sky-600">∞</span>;
    if (id === 'series-inv' || id === 'shunt-inv') return <Zap className="w-5 h-5" />;
    if (id === 'microgrid') return <Sun className="w-5 h-5 text-amber-600" />;
    if (id === 'scope-block') return <Eye className="w-5 h-5 animate-pulse text-sky-600" />;
    if (id.includes('mppt')) return <Cpu className="w-5 h-5 text-amber-600" />;
    if (id.includes('battery') || id.includes('bess')) return <span className="text-lg font-black text-emerald-600">⚡</span>;
    if (id.includes('wind') || id.includes('turbine')) return <span className="text-lg font-black text-cyan-600">🌀</span>;
    return <Cpu className="w-5 h-5" />;
  };

  // Show dimmed/dashed border if node is marked inactive (disconnected) or explicitly tripped
  const isTripped = String(data.parameters?.isTripped) === 'true';
  const isDisconnected = data.active === false || isTripped;

  return (
    <div className={`p-2 rounded border-2 shadow-lg transition-all font-mono select-none min-w-[140px]
      ${isDisconnected ? 'border-slate-200 bg-slate-50 text-slate-300 opacity-50 border-dashed' : getNodeStyling()}
      ${selected ? 'ring-2 ring-cyan-400 scale-105' : ''}`}>
      
      {/* Left */}
      <Handle type="target" position={Position.Left} id="l-t" style={{ background: 'transparent', border: 'none', width: '1px', height: '1px' }} />
      <Handle type="source" position={Position.Left} id="l-s" style={{ background: '#00f0ff', width: '8px', height: '8px', borderRadius: '50%' }} />
      {/* Right */}
      <Handle type="target" position={Position.Right} id="r-t" style={{ background: 'transparent', border: 'none', width: '1px', height: '1px' }} />
      <Handle type="source" position={Position.Right} id="r-s" style={{ background: '#00f0ff', width: '8px', height: '8px', borderRadius: '50%' }} />
      {/* Top */}
      <Handle type="target" position={Position.Top} id="t-t" style={{ background: 'transparent', border: 'none', width: '1px', height: '1px' }} />
      <Handle type="source" position={Position.Top} id="t-s" style={{ background: '#f59e0b', width: '8px', height: '8px', borderRadius: '50%' }} />
      {/* Bottom */}
      <Handle type="target" position={Position.Bottom} id="b-t" style={{ background: 'transparent', border: 'none', width: '1px', height: '1px' }} />
      <Handle type="source" position={Position.Bottom} id="b-s" style={{ background: '#f59e0b', width: '8px', height: '8px', borderRadius: '50%' }} />


      <div className="flex items-center space-x-2">
        <div className="flex-shrink-0">{renderIcon()}</div>
        <div>
          <div className="text-[10px] font-bold tracking-wide uppercase leading-tight">{data.label}</div>
          {data.details && (
            <div className="text-[8px] opacity-80 whitespace-pre-line leading-none mt-0.5">{data.details}</div>
          )}
        </div>
      </div>
    </div>
  );
};

const nodeTypes: NodeTypes = {
  electrical: CustomElectricalNode,
  control: CustomElectricalNode,
  microgrid: CustomElectricalNode,
  scope: CustomElectricalNode,
};

interface SimulationCanvasProps {
  isSimulating: boolean;
  selectedNodeId: string | null;
  onSelectNode: (nodeId: string | null) => void;
  setZoomInRef: React.MutableRefObject<(() => void) | null>;
  setZoomOutRef: React.MutableRefObject<(() => void) | null>;
  setFitViewRef: React.MutableRefObject<(() => void) | null>;
  getTopologyRef: React.MutableRefObject<(() => { nodes: any[]; edges: any[] }) | null>;
  setUpdateNodeRef: React.MutableRefObject<((id: string, data: any) => void) | null>;
  onNodeDoubleClick?: (event: React.MouseEvent, node: any) => void;
  /** Called whenever edges change — allows parent to reactively update connectedNodeIds */
  onTopologyChange?: (nodeIds: Set<string>, edges: any[]) => void;
}

export default function SimulationCanvas({
  isSimulating,
  selectedNodeId,
  onSelectNode,
  setZoomInRef,
  setZoomOutRef,
  setFitViewRef,
  getTopologyRef,
  setUpdateNodeRef,
  onNodeDoubleClick,
  onTopologyChange,
}: SimulationCanvasProps) {
  const initialNodesState = useMemo(() => initialNodes, []);
  const initialEdgesState = useMemo(() => initialEdges, []);

  const [nodes, setNodes, onNodesChange] = useNodesState(initialNodesState);
  const [edges, setEdges, onEdgesChange] = useEdgesState(initialEdgesState);
  const { zoomIn, zoomOut, fitView, screenToFlowPosition } = useReactFlow();
  const reactFlowWrapper = useRef<HTMLDivElement>(null);
  const [loaded, setLoaded] = useState(false);
  
  // Hydration-safe load from localStorage
  useEffect(() => {
    const saved = localStorage.getItem('solar_twin_topology');
    if (saved) {
      try {
        const parsed = JSON.parse(saved);
        if (parsed.nodes && parsed.nodes.length > 0) setNodes(parsed.nodes);
        if (parsed.edges) setEdges(parsed.edges);
      } catch(e) {}
    }
    setLoaded(true);
  }, [setNodes, setEdges]);

  useEffect(() => {
    setZoomInRef.current = () => zoomIn({ duration: 300 });
    setZoomOutRef.current = () => zoomOut({ duration: 300 });
    setFitViewRef.current = () => fitView({ duration: 300, padding: 0.1 });
    getTopologyRef.current = () => ({ nodes, edges });
    setUpdateNodeRef.current = (id: string, params: any) => {
      setNodes((nds) => nds.map((n) => (n.id === id ? { ...n, data: { ...n.data, parameters: params } } : n)));
    };
  }, [zoomIn, zoomOut, fitView, setZoomInRef, setZoomOutRef, setFitViewRef, getTopologyRef, setUpdateNodeRef, nodes, edges, setNodes]);

  // Notify parent of topology changes when edges or nodes change
  useEffect(() => {
    if (onTopologyChange) {
      const ids = new Set(edges.flatMap((e: any) => [e.source, e.target]));
      onTopologyChange(ids, edges);
    }
  }, [edges, nodes, onTopologyChange]); // FIX BUG-F02: onTopologyChange is stable (useCallback in parent)

  // Save topology to localStorage on any change so it persists when returning from dashboard
  useEffect(() => {
    if (loaded) {
      localStorage.setItem('solar_twin_topology', JSON.stringify({ nodes, edges }));
    }
  }, [nodes, edges, loaded]);

  // Separate effect: update battery/wind node active flags when edges change ONLY
  // (kept separate from the main effect to avoid the nodes→setNodes infinite loop)
  useEffect(() => {
    const OPTIONAL = ['battery', 'wind'];
    const connectedSet = new Set(edges.flatMap((e: any) => [e.source, e.target]));
    setNodes((nds) => {
      let changed = false;
      const next = nds.map((n) => {
        if (!OPTIONAL.some(kw => n.id.includes(kw))) return n;
        const shouldBeActive = connectedSet.has(n.id);
        if (!!n.data.active === shouldBeActive) return n;
        changed = true;
        return { ...n, data: { ...n.data, active: shouldBeActive } };
      });
      return changed ? next : nds; // return same reference if nothing changed → no re-render
    });
  }, [edges, setNodes]);

  const onConnect = useCallback(
    (params: Edge | Connection) => setEdges((eds) => addEdge({ ...params, animated: true, style: { stroke: '#0ea5e9', strokeWidth: 2 } }, eds)),
    [setEdges]
  );

  // Double-click an edge to disconnect (remove it from topology)
  const onEdgeDoubleClick = useCallback(
    (_event: React.MouseEvent, edge: Edge) => {
      setEdges((eds) => eds.filter((e) => e.id !== edge.id));
    },
    [setEdges]
  );


  const onDragOver = useCallback((event: React.DragEvent) => {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'move';
  }, []);

  const onDrop = useCallback(
    (event: React.DragEvent) => {
      event.preventDefault();

      const typeData = event.dataTransfer.getData('application/reactflow');
      if (!typeData) return;

      const { type, label } = JSON.parse(typeData);

      // FIX BUG-11: screenToFlowPosition() replaces deprecated project().
      // Takes raw screen coords directly — no bounding rect subtraction needed.
      const position = screenToFlowPosition({
        x: event.clientX,
        y: event.clientY,
      });

      const newNode = {
        id: `node_${new Date().getTime()}`,
        // FIX BUG-F05: Default to 'electrical' if type is not in registered nodeTypes
        type: ['electrical', 'control', 'microgrid', 'scope'].includes(type) ? type : 'electrical',
        position,
        data: { label, type, details: 'Dynamically added component' },
      };

      setNodes((nds) => nds.concat(newNode));
    },
    [screenToFlowPosition, setNodes]
  );


  return (
    <div className="flex-1 h-full bg-[#F8FAFC] relative overflow-hidden select-none" ref={reactFlowWrapper} onDrop={onDrop} onDragOver={onDragOver}>
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        onNodeClick={(event, node) => onSelectNode(node.id)}
        onNodeDoubleClick={onNodeDoubleClick}
        onEdgeDoubleClick={onEdgeDoubleClick}
        onPaneClick={() => onSelectNode(null)}
        deleteKeyCode="Delete"
        multiSelectionKeyCode="Shift"
        edgesUpdatable={true}
        nodeTypes={nodeTypes}
        fitView
        fitViewOptions={{ padding: 0.1 }}
      >
        <Background color="#e2e8f0" gap={18} size={1} />

        {/* Canvas HUD matching exact screenshot design */}
        <Panel position="top-left" className="m-3 p-3 bg-white/95 text-slate-800 rounded-md border border-slate-200 font-mono text-[10px] space-y-1 shadow-md max-w-[220px]">
          <div className="font-bold text-sky-600 uppercase border-b border-slate-200 pb-1 mb-1 tracking-wider text-[9px]">
            TOPOLOGY STATUS
          </div>
          {(() => {
            const connectedIds = new Set(edges.flatMap(e => [e.source, e.target]));
            const hasBattery = [...connectedIds].some(id => id.includes('battery') || id.includes('bess'));
            const hasWind = [...connectedIds].some(id => id.includes('wind') || id.includes('turbine'));
            const solarNodes = nodes.filter(n => connectedIds.has(n.id) && (n.id.includes('solar') || n.id.includes('microgrid') || n.id.includes('pv')));
            return (
              <>
                <div className="flex items-center justify-between">
                  <span className="opacity-60">Nodes</span>
                  <span className="font-bold text-slate-700">{nodes.length} / edges: {edges.length}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="opacity-60">☀️ Solar</span>
                  <span className={solarNodes.length > 0 ? 'text-amber-600 font-bold' : 'text-slate-400'}>{solarNodes.length > 0 ? `${solarNodes.length} array(s) connected` : 'not connected'}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="opacity-60">⚡ Battery</span>
                  <span className={hasBattery ? 'text-emerald-600 font-bold' : 'text-slate-400'}>{hasBattery ? '∥ DC Bus' : 'not connected'}</span>
                </div>
                <div className="flex items-center justify-between">
                  <span className="opacity-60">🌀 Wind</span>
                  <span className={hasWind ? 'text-cyan-600 font-bold' : 'text-slate-400'}>{hasWind ? '∥ AC Bus' : 'not connected'}</span>
                </div>
                <div className="mt-1 border-t border-slate-100 pt-1 text-[8px] text-slate-400">
                  Double-click a wire to disconnect
                </div>
              </>
            );
          })()}
        </Panel>
      </ReactFlow>
    </div>
  );
}
