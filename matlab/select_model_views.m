clear all; clc;
addpath('util');
addpath('dataset/SHREC2013');

SKETCH_DIR = 'dataset/SHREC2013/Sketches';
MODEL_DIR = 'dataset/SHREC2013/Models';

[sketchclass, Ns] = read_classification_file_shrec13('dataset/SHREC2013/SHREC13_SBR_Sketch_Test.cla'); 
[modelclass, N] = read_classification_file_shrec13('dataset/SHREC2013/SHREC13_SBR_Model.cla');

% mexSelectMeshViews('init_context');
% for i = [997,1214,123,1025,1140,796,1007,1205,611,1222,688,199,1152,825,531,351,1150,160,179,1208]
% for i = 1:size(modelclass, 1)
% for i = 25:800
%     target_model = ['m' modelclass{i, 1}];
%     target_class = modelclass{i, 2};
%     
%     idx = strcmp(sketchclass(:, 2), target_class);
%     target_sketch = sketchclass(idx, 1);
%     target_sketch_path = strcat(SKETCH_DIR, ['/' target_class '/test/'], target_sketch, '.png');
%     target_model_path = [MODEL_DIR '/' target_model '.off'];
%     
%     tic;
%     mexSelectMeshViews('select_mesh', target_model_path, target_model, target_sketch_path, 'Views');
%     dt = toc;
%     fprintf('Done #%d in %f seconds.\n', i, dt);
%     drawnow('update');
% end
% mexSelectMeshViews('clear_context');

model_num = size(modelclass, 1);
fid = fopen('model.txt', 'w');
fprintf(fid, '%d\n', model_num);
for i = 1:model_num
    target_model = ['m' modelclass{i, 1}];
    target_class = modelclass{i, 2};
    target_model_path = ['../' MODEL_DIR '/' target_model '.off'];
    
    fprintf(fid, '%s\t%s\t%s\n', target_model_path, target_class, target_model);
end
fclose(fid);

sketch_num = size(sketchclass, 1);
fid = fopen('sketch.txt', 'w');
fprintf(fid, '%d\n', sketch_num);
for i = 1:sketch_num    
	target_sketch = sketchclass{i, 1};
    target_class = sketchclass{i, 2};
	target_sketch_path = strcat(['../' SKETCH_DIR], ['/' target_class '/test/'], target_sketch, '.png');
    
    fprintf(fid, '%s\t%s\n', target_sketch_path, target_class);
end
fclose(fid);
