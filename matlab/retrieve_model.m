clear all;
clc;

DATASET_DIR = 'SHREC2013';
DATASET_VIEW_DIR = fullfile(DATASET_DIR, 'Views');
DATASET_SKETCH_DIR = fullfile(DATASET_DIR, 'Sketches');
DATASET_CONFIG_SKETCH_FILE = fullfile(DATASET_DIR, 'SHREC13_SBR_Sketch_Test.cla');
DATASET_CONFIG_MODEL_FILE = fullfile(DATASET_DIR, 'SHREC13_SBR_Model.cla');

RANK_DIR = fullfile(DATASET_DIR, 'RetrievalLists');
INDEXER_DATA_FILE = fullfile(DATASET_DIR, 'Cache', 'indexerdata.dat');

addpath('util');
addpath(DATASET_DIR);

[sketchclass, ~] = read_classification_file_shrec13(DATASET_CONFIG_SKETCH_FILE);
[modelclass, ~] = read_classification_file_shrec13(DATASET_CONFIG_MODEL_FILE);
modelclass(:, 1) = strcat('m', modelclass(:, 1));

if ~exist(RANK_DIR, 'dir')
    mkdir(RANK_DIR);
end

%%
% mexHashIndexer('init');
% viewfiles = dir(fullfile(DATASET_VIEW_DIR, '*.png'));
% target_num = size(viewfiles, 1);
% for i = 1:target_num 
%     tic;
%     target_path = viewfiles(i).name;
%     [~, model_name, ~] = fileparts(target_path);
%     mexHashIndexer('insert', fullfile(DATASET_VIEW_DIR, target_path), model_name);
%     dt = toc;
%     fprintf('Done #%d in %f seconds.\n', i, dt);
% end
% mexHashIndexer('save', INDEXER_DATA_FILE);

%%
mexHashIndexer('load', INDEXER_DATA_FILE);
query_num = size(sketchclass, 1);
t = 0;
for i = 1:query_num
    tic;
    query_name = sketchclass{i, 1};
    query_class = sketchclass{i, 2};
    target_query_path = fullfile(DATASET_SKETCH_DIR, query_class, 'test', [query_name, '.png']);
    mexHashIndexer('retrieve_model', target_query_path, query_name, fullfile(RANK_DIR, query_name));
    dt = toc;
    t = t + dt;
    fprintf('Done #%d in %f seconds.\n', i, dt);
end
fprintf('Average %f s/m.\n', t / query_num);
