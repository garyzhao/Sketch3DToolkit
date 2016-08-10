clear all;
clc;

DATASET_DIR = 'SMTPG2015';
DATASET_DATA_DIR = fullfile(DATASET_DIR, 'Data');
DATASET_CONFIG_FILE = fullfile(DATASET_DIR, 'SMTPG2015_SBR_Sketch.mat');

RANK_DIR = fullfile(DATASET_DIR, 'RetrievalLists');
INDEXER_DATA_FILE = fullfile(DATASET_DIR, 'Cache', 'indexerdata.dat');

addpath('util');
addpath(DATASET_DIR);
[targetclass, ~, queryclass, ~] = setup_classification_data(DATASET_DATA_DIR, DATASET_CONFIG_FILE);

if ~exist(RANK_DIR, 'dir')
    mkdir(RANK_DIR);
end

%%
mexHashIndexer('init');
target_num = size(targetclass, 1);
for i = 1:target_num 
    tic;
    target_name = targetclass{i, 1};
    mexHashIndexer('insert', fullfile(DATASET_DATA_DIR, [target_name '.png']), target_name);
    dt = toc;
    fprintf('Done #%d in %f seconds.\n', i, dt);
end
mexHashIndexer('save', INDEXER_DATA_FILE);

%%
query_num = size(queryclass, 1);
for i = 1:query_num
    tic;
    query_name = queryclass{i, 1};
    mexHashIndexer('retrieve_sketch', fullfile(DATASET_DATA_DIR, [query_name '.png']), query_name, fullfile(RANK_DIR, query_name));
    dt = toc;
    fprintf('Done #%d in %f seconds.\n', i, dt);
end
