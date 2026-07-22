%% build_DT_Advanced.m
% ------------------------------------------------------------------
% Builds DT_Advanced.slx: a two-part solar-PV power-quality digital
% twin designed to generate rich, PRECISELY LABELED time-series data
% for training an LSTM classifier/predictor. Saves natively in
% whatever MATLAB release you run this in (target: R2020a).
%
% MODELING APPROACH (read this before you run it):
%   PART 1 - DC/PV side  -> genuinely physically simulated
%     Real Simscape Electrical "PV Array" block + a closed-loop
%     Perturb & Observe MPPT controller (MATLAB Function block) +
%     an AVERAGED boost-converter stage (Controlled Voltage Source
%     driven by Vout = Vpv/(1-D)). Averaged, not switching-level,
%     so it stays on a continuous solver and is fast enough to run
%     hundreds of times for a training set, while still capturing
%     real PV nonlinearity + MPPT transient dynamics.
%   PART 2 - AC/grid side -> scripted IEEE-1159-style PQ disturbances
%     Three independent per-phase circuits where the source voltage
%     waveform is generated in MATLAB (sag/swell/harmonics/
%     interruption/normal) and fed in via Controlled Voltage Source
%     blocks; current is a genuine physical response through real
%     RL line + load impedances, and gets measured normally. This
%     is the same waveform-synthesis approach used in most published
%     PQ-classification datasets, and it gives you exact, jitter-free
%     disturbance onset/offset times for labeling.
%   These two parts are logged with SEPARATE scenario labels
%   (pv_scenario, grid_scenario) so you can train one joint LSTM or
%   two specialized ones.
%
% Why not a full switching inverter (Universal Bridge + PWM)? It's
% the more "textbook accurate" choice, but it forces a discrete
% solver with a small fixed step, which is slow to run hundreds of
% times for a training set, and its exact port layout depends on
% mask settings I can't verify without running MATLAB myself. If you
% want that fidelity for your final proposal deck (not the training
% loop), tell me and I'll add it as a separate high-fidelity variant.
%
% REQUIREMENTS: Simulink + Simscape Electrical / Specialized Power
% Systems (formerly SimPowerSystems), licensed in your R2020a.
%
% HOW TO RUN:
%   1. Open MATLAB R2020a, cd to this file's folder
%   2. Run:  build_DT_Advanced
%   3. DT_Advanced.slx is created + opened in the current folder
%   4. Then run run_batch_scenarios.m to generate labeled training
%      data (many runs, randomized disturbance parameters) as one
%      combined CSV ready for LSTM windowing in Python.
% ------------------------------------------------------------------

function build_DT_Advanced()

modelName = 'DT_Advanced';
if bdIsLoaded(modelName), close_system(modelName, 0); end
if exist([modelName '.slx'], 'file'), delete([modelName '.slx']); end

new_system(modelName);
open_system(modelName);
set_param(modelName, 'Solver', 'ode23tb', 'StopTime', '10');

%% ================= PART 1: DC / PV SIDE =================
add_block('simulink/Sources/From Workspace', [modelName '/Irradiance_Profile'], ...
    'Position', [30 30 130 70], 'VariableName', 'irradianceProfile');
add_block('simulink/Sources/Constant', [modelName '/Cell_Temperature'], ...
    'Position', [30 100 130 140], 'Value', '25');
add_block('simulink/Sources/From Workspace', [modelName '/PV_Scenario_Label'], ...
    'Position', [30 170 130 210], 'VariableName', 'pvScenarioLabel');

pvPath = locate_block('PV Array');
try
    add_block(pvPath, [modelName '/PV_Array'], 'Position', [200 30 300 130]);
    pvAdded = true;
catch ME
    warn_manual('PV Array', ME);
    pvAdded = false;
end

add_block('powerlib/Measurements/Voltage Measurement', [modelName '/Vpv_Meas'], ...
    'Position', [370 20 430 60]);
add_block('powerlib/Measurements/Current Measurement', [modelName '/Ipv_Meas'], ...
    'Position', [370 90 430 130]);
add_block('powerlib/Elements/Series RLC Branch', [modelName '/PV_Nominal_Load'], ...
    'Position', [500 90 560 130], 'BranchType', 'R', 'R', '8');
add_block('powerlib/Elements/Ground', [modelName '/Ground_PV'], ...
    'Position', [560 160 590 190]);

% --- MPPT (Perturb & Observe), closed loop on real PV Array output ---
add_block('simulink/User-Defined Functions/MATLAB Function', [modelName '/MPPT_PO'], ...
    'Position', [430 200 560 260]);
set_chart_script(modelName, 'MPPT_PO', strjoin({
    'function [D, P] = mppt_po(V, I)'
    '%#codegen'
    'persistent D_prev V_prev P_prev'
    'if isempty(D_prev)'
    '    D_prev = 0.5; V_prev = 0; P_prev = 0;'
    'end'
    'step = 0.001;'
    'P = V * I;'
    'dV = V - V_prev;'
    'dP = P - P_prev;'
    'if dP ~= 0'
    '    if (dP > 0 && dV > 0) || (dP < 0 && dV < 0)'
    '        D = D_prev + step;'
    '    else'
    '        D = D_prev - step;'
    '    end'
    'else'
    '    D = D_prev;'
    'end'
    'D = min(max(D, 0.1), 0.9);'
    'D_prev = D; V_prev = V; P_prev = P;'
    'end'
}, newline));

% --- Averaged boost stage: Vout = Vpv / (1 - D) ---
add_block('simulink/User-Defined Functions/MATLAB Function', [modelName '/Boost_Avg'], ...
    'Position', [620 200 750 260]);
set_chart_script(modelName, 'Boost_Avg', strjoin({
    'function Vout = boost_avg(Vin, D)'
    '%#codegen'
    'Vout = Vin / max(1 - D, 0.15);'
    'end'
}, newline));

boostSrcPath = locate_block('Controlled Voltage Source');
try
    add_block(boostSrcPath, [modelName '/Boosted_DC_Source'], 'Position', [800 190 860 260]);
    boostAdded = true;
catch ME
    warn_manual('Controlled Voltage Source (boost)', ME);
    boostAdded = false;
end

add_block('powerlib/Elements/Series RLC Branch', [modelName '/DC_Link_Load'], ...
    'Position', [930 190 990 230], 'BranchType', 'R', 'R', '20');
add_block('powerlib/Measurements/Voltage Measurement', [modelName '/DC_Link_V_Meas'], ...
    'Position', [930 260 990 300]);
add_block('powerlib/Elements/Ground', [modelName '/Ground_Boost'], ...
    'Position', [990 320 1020 350]);

% --- PV-side logging ---
add_block('simulink/Sinks/To Workspace', [modelName '/Log_PV_Voltage'], ...
    'Position', [430 20 530 50], 'VariableName', 'dt_pv_voltage', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_PV_Current'], ...
    'Position', [430 90 530 120], 'VariableName', 'dt_pv_current', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_PV_Power'], ...
    'Position', [620 280 720 310], 'VariableName', 'dt_pv_power', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_Duty_Cycle'], ...
    'Position', [620 340 720 370], 'VariableName', 'dt_duty_cycle', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_DC_Link_Voltage'], ...
    'Position', [1050 260 1150 290], 'VariableName', 'dt_dc_link_voltage', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_PV_Scenario'], ...
    'Position', [30 240 130 270], 'VariableName', 'dt_pv_scenario_label', 'SaveFormat', 'Structure With Time');

%% ================= PART 2: AC / GRID PQ SIDE =================
add_block('simulink/Sources/From Workspace', [modelName '/Grid_Scenario_Label'], ...
    'Position', [30 450 130 480], 'VariableName', 'gridScenarioLabel');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_Grid_Scenario'], ...
    'Position', [230 450 330 480], 'VariableName', 'dt_grid_scenario_label', 'SaveFormat', 'Structure With Time');
try
    add_line(modelName, 'Grid_Scenario_Label/1', 'Log_Grid_Scenario/1', 'autorouting', 'on');
catch, end

vMeasHandles = cell(1,3);
iMeasHandles = cell(1,3);
phases = {'A','B','C'};
for k = 1:3
    yBase = 520 + (k-1) * 160;
    [vMeasHandles{k}, iMeasHandles{k}] = add_phase_circuit(modelName, phases{k}, yBase);
end

add_block('simulink/Signal Routing/Mux', [modelName '/Mux_Vabc'], ...
    'Position', [900 560 930 660], 'Inputs', '3');
add_block('simulink/Signal Routing/Mux', [modelName '/Mux_Iabc'], ...
    'Position', [900 700 930 800], 'Inputs', '3');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_Grid_Vabc'], ...
    'Position', [970 590 1070 620], 'VariableName', 'dt_grid_vabc', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/To Workspace', [modelName '/Log_Grid_Iabc'], ...
    'Position', [970 730 1070 760], 'VariableName', 'dt_grid_iabc', 'SaveFormat', 'Structure With Time');
add_block('simulink/Sinks/Scope', [modelName '/Scope_Grid_PQ'], ...
    'Position', [970 650 1030 700], 'NumInputPorts', '2');

try
    for k = 1:3
        add_line(modelName, [vMeasHandles{k} '/1'], ['Mux_Vabc/' num2str(k)], 'autorouting', 'on');
        add_line(modelName, [iMeasHandles{k} '/1'], ['Mux_Iabc/' num2str(k)], 'autorouting', 'on');
    end
    add_line(modelName, 'Mux_Vabc/1', 'Log_Grid_Vabc/1', 'autorouting', 'on');
    add_line(modelName, 'Mux_Iabc/1', 'Log_Grid_Iabc/1', 'autorouting', 'on');
    add_line(modelName, 'Mux_Vabc/1', 'Scope_Grid_PQ/1', 'autorouting', 'on');
    add_line(modelName, 'Mux_Iabc/1', 'Scope_Grid_PQ/2', 'autorouting', 'on');
catch ME
    warning('DT_Advanced:GridMux', 'Grid mux/logging wiring incomplete (%s). Finish manually.', ME.message);
end

%% ================= powergui (mandatory for any SPS model) =================
add_block('powerlib/powergui', [modelName '/powergui'], ...
    'Position', [30 900 160 940], 'Simulationmode', 'Continuous');

%% ================= PV-side wiring =================
try
    if pvAdded
        add_line(modelName, 'Irradiance_Profile/1', 'PV_Array/1', 'autorouting', 'on');
        add_line(modelName, 'Cell_Temperature/1', 'PV_Array/2', 'autorouting', 'on');
        add_line(modelName, 'PV_Array/1', 'Vpv_Meas/1', 'autorouting', 'on');
        add_line(modelName, 'PV_Array/1', 'Ipv_Meas/1', 'autorouting', 'on');
    end
    add_line(modelName, 'Ipv_Meas/2', 'PV_Nominal_Load/1', 'autorouting', 'on');

    add_line(modelName, 'Vpv_Meas/1', 'Log_PV_Voltage/1', 'autorouting', 'on');
    add_line(modelName, 'Ipv_Meas/1', 'Log_PV_Current/1', 'autorouting', 'on');
    add_line(modelName, 'Vpv_Meas/1', 'MPPT_PO/1', 'autorouting', 'on');
    add_line(modelName, 'Ipv_Meas/1', 'MPPT_PO/2', 'autorouting', 'on');
    add_line(modelName, 'MPPT_PO/2', 'Log_PV_Power/1', 'autorouting', 'on');
    add_line(modelName, 'MPPT_PO/1', 'Log_Duty_Cycle/1', 'autorouting', 'on');

    add_line(modelName, 'Vpv_Meas/1', 'Boost_Avg/1', 'autorouting', 'on');
    add_line(modelName, 'MPPT_PO/1', 'Boost_Avg/2', 'autorouting', 'on');
    if boostAdded
        add_line(modelName, 'Boost_Avg/1', 'Boosted_DC_Source/1', 'autorouting', 'on');
        add_line(modelName, 'Boosted_DC_Source/1', 'DC_Link_Load/1', 'autorouting', 'on');
        add_line(modelName, 'Boosted_DC_Source/1', 'DC_Link_V_Meas/1', 'autorouting', 'on');
    end
    add_line(modelName, 'DC_Link_V_Meas/1', 'Log_DC_Link_Voltage/1', 'autorouting', 'on');
    add_line(modelName, 'PV_Scenario_Label/1', 'Log_PV_Scenario/1', 'autorouting', 'on');
catch ME
    warning('DT_Advanced:PVWiring', 'Some PV-side connections need manual finishing (%s).', ME.message);
end

%% ================= Scenario timeline (defaults for a single 10s run) =================
[irr, pvLbl] = default_pv_scenario();
assignin('base', 'irradianceProfile', irr);
assignin('base', 'pvScenarioLabel', pvLbl);

[vA, vB, vC, gridLbl] = default_grid_scenario();
assignin('base', 'gridVaRef', vA);
assignin('base', 'gridVbRef', vB);
assignin('base', 'gridVcRef', vC);
assignin('base', 'gridScenarioLabel', gridLbl);

%% ================= Save natively in the running MATLAB's format =================
save_system(modelName, fullfile(pwd, [modelName '.slx']));
fprintf('\n%s.slx saved to: %s\n', modelName, fullfile(pwd, [modelName '.slx']));
fprintf('This default run has one scenario timeline built in. For an actual\n');
fprintf('LSTM training SET, run run_batch_scenarios.m next -- it re-randomizes\n');
fprintf('these same workspace variables across many runs and exports one\n');
fprintf('combined, labeled CSV.\n');

end

%% ------------------------------------------------------------------
function [vMeasName, iMeasName] = add_phase_circuit(modelName, ph, yBase)
% Builds one phase of the grid PQ side: a scripted voltage reference
% -> Controlled Voltage Source -> line RL -> PCC (V + I measured) ->
% resistive load -> ground. Returns block names for the V/I
% measurement blocks so the caller can mux all three phases.
srcVar = ['gridV' lower(ph) 'ref'];
b = @(suffix) [suffix '_' ph];

add_block('simulink/Sources/From Workspace', [modelName '/' b('Vref')], ...
    'Position', [30 yBase 130 yBase+40], 'VariableName', srcVar);

srcPath = locate_block('Controlled Voltage Source');
srcAdded = true;
try
    add_block(srcPath, [modelName '/' b('Source')], 'Position', [200 yBase-10 260 yBase+50]);
catch ME
    warn_manual(['Controlled Voltage Source (phase ' ph ')'], ME);
    srcAdded = false;
end

add_block('powerlib/Elements/Series RLC Branch', [modelName '/' b('Line')], ...
    'Position', [330 yBase 400 yBase+40], 'BranchType', 'RL', 'R', '0.5', 'L', '1e-3');
add_block('powerlib/Measurements/Current Measurement', [modelName '/' b('IMeas')], ...
    'Position', [430 yBase 490 yBase+40]);
add_block('powerlib/Measurements/Voltage Measurement', [modelName '/' b('VMeas')], ...
    'Position', [560 yBase-40 620 yBase], 'Orientation', 'up');
add_block('powerlib/Elements/Series RLC Branch', [modelName '/' b('Load')], ...
    'Position', [560 yBase+50 620 yBase+90], 'BranchType', 'R', 'R', '50');
add_block('powerlib/Elements/Ground', [modelName '/' b('Gnd')], ...
    'Position', [620 yBase+110 650 yBase+140]);

try
    if srcAdded
        add_line(modelName, [b('Vref') '/1'], [b('Source') '/1'], 'autorouting', 'on');
        add_line(modelName, [b('Source') '/1'], [b('Line') '/1'], 'autorouting', 'on');
    end
    add_line(modelName, [b('Line') '/2'], [b('IMeas') '/1'], 'autorouting', 'on');
    add_line(modelName, [b('IMeas') '/2'], [b('VMeas') '/1'], 'autorouting', 'on');
    add_line(modelName, [b('IMeas') '/2'], [b('Load') '/1'], 'autorouting', 'on');
catch ME
    warning('DT_Advanced:PhaseWiring', 'Phase %s wiring needs manual finishing (%s).', ph, ME.message);
end

vMeasName = b('VMeas');
iMeasName = b('IMeas');
end

%% ------------------------------------------------------------------
function [irr, lbl] = default_pv_scenario()
% Same 4-segment PV profile from DT_Base, coded for logging:
% 0=Normal 1=Startup_Transient 2=Irradiance_Sag 3=Recovery_Transient
t   = [0; 2; 2.001; 3; 3.001; 6; 6.001; 7; 7.001; 8; 8.001; 10];
irr_v = [1000; 1000; 0; 1000; 1000; 1000; 400; 400; 400; 1000; 1000; 1000];
lbl_v = [0;    0;    1; 1;    0;    0;    2;   2;   3;   3;    0;    0];
irr = [t, irr_v];
lbl = [t, lbl_v];
end

%% ------------------------------------------------------------------
function [vA, vB, vC, lbl] = default_grid_scenario()
% Synthesizes one 10s three-phase reference with a single grid PQ
% event for a quick default run. run_batch_scenarios.m replaces this
% with randomized events across many runs for real LSTM training.
% Label codes: 0=Normal 1=Sag 2=Swell 3=Harmonics 4=Interruption
Fs = 5000; f0 = 50; Vnom = 230*sqrt(2);
t = (0:1/Fs:10)';
mag = ones(size(t));
harm = zeros(size(t));
lblCode = zeros(size(t));

sagMask = t >= 4 & t < 4.5;
mag(sagMask) = 0.55;
lblCode(sagMask) = 1;

swellMask = t >= 6 & t < 6.4;
mag(swellMask) = 1.35;
lblCode(swellMask) = 2;

harmMask = t >= 7.5 & t < 8.2;
harm(harmMask) = 0.15*sin(2*pi*5*f0*t(harmMask)) + 0.10*sin(2*pi*7*f0*t(harmMask));
lblCode(harmMask) = 3;

interMask = t >= 9 & t < 9.2;
mag(interMask) = 0.05;
lblCode(interMask) = 4;

vA = [t, Vnom*mag.*sin(2*pi*f0*t) + Vnom*harm];
vB = [t, Vnom*mag.*sin(2*pi*f0*t - 2*pi/3) + Vnom*harm];
vC = [t, Vnom*mag.*sin(2*pi*f0*t + 2*pi/3) + Vnom*harm];
lbl = [t, lblCode];
end

%% ------------------------------------------------------------------
function bp = locate_block(blockName)
% Searches loaded Simulink libraries for a block by name so we don't
% hardcode a submenu path that may differ across toolbox versions.
libsToTry = {'powerlib', 'sps_lib', 'renewables_lib', 'physmod_sps_lib'};
for i = 1:numel(libsToTry)
    try, load_system(libsToTry{i}); catch, end
end
found = find_system('type', 'block_diagram', 'LibraryType', 'BlockLibrary');
bp = '';
for i = 1:numel(found)
    hits = find_system(found{i}, 'FollowLinks', 'on', 'LookUnderMasks', 'all', 'Name', blockName);
    if ~isempty(hits), bp = hits{1}; return; end
end
error('locate_block:NotFound', 'Block "%s" not found in any loaded library.', blockName);
end

%% ------------------------------------------------------------------
function warn_manual(blockName, ME)
warning('DT_Advanced:BlockMissing', ...
    ['Could not add "%s" automatically (%s).\n' ...
     'Open the Simulink Library Browser, search for it, drag it in manually,\n' ...
     'and wire it into the surrounding blocks already placed on the canvas.'], ...
    blockName, ME.message);
end

%% ------------------------------------------------------------------
function set_chart_script(modelName, blockName, scriptText)
% Programmatically writes MATLAB code into a MATLAB Function block.
blockPath = [modelName '/' blockName];
chart = find(sfroot, '-isa', 'Stateflow.EMChart', 'Path', blockPath);
if isempty(chart)
    warning('DT_Advanced:ChartNotFound', ...
        'Could not locate MATLAB Function block "%s" to set its code -- paste it in manually:\n%s', ...
        blockName, scriptText);
    return;
end
chart.Script = scriptText;
end