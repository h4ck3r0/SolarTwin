function run_model()

model = 'upqc5_7_1_PI_20a_fin_1_1';

disp('=== Starting simulation ===');

simOut = sim(model);

disp('=== Simulation completed successfully ===');
disp(['Final simulation time: ' num2str(simOut.tout(end))]);