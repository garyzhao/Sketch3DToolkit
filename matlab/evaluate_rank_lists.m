%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%                     The evaluation code for 
%%% SHREC'12 -- Sketch-Based 3D Shape Retrieval   
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%Input:
%       rank list files (training or testing dataset by changing the 3rd
%       clause)
%       
%       classification files (training or testing dataset, target model dataset) 

%Output:
%       Precision Recall performance                         -- "PR.txt"
%       Other performance metrics (each model and average)   -- "Stats.txt"

%Evaluation measures:
%       NN, 1-Tier, 2-Tier, e_Measure, DCG

%Author: Bo Li
%Email: li.bo.ntu0@gmail.com, libo0002@ntu.edu.sg, b_l58@txstate.edu
%Date: January 24, 2012
%@Texas State University, Texas, US

%Please cite:
% SHREC'13 -- Large Sketch-Based 3D Shape Retrieval, 3DOR'13, 2013

%Note: please note the comments started with "%Chang..." to adjust to your
%cases

%% Setup variables.
clearvars; clc;

% % Change the folder that contains the rank list files accordingly.
% DATASET_DIR = 'SMTPG2015';
% DATASET_DATA_DIR = fullfile(DATASET_DIR, 'Data');
% DATASET_CONFIG_FILE = fullfile(DATASET_DIR, 'SMTPG2015_SBR_Sketch.mat');
% 
% RANK_DIR = fullfile(DATASET_DIR, 'RetrievalLists');
% RANK_STATS_FILE = fullfile(RANK_DIR, 'Stats.txt');
% RANK_PRC_FILE = fullfile(RANK_DIR, 'PR.txt');
% 
% addpath('mex');
% addpath('util');
% addpath(DATASET_DIR);
% 
% % Load dataset configuration.
% [targetClass, N, queryClass, ~] = setup_classification_data(DATASET_DATA_DIR, DATASET_CONFIG_FILE);

DATASET_DIR = 'SHREC2013';
DATASET_CONFIG_SKETCH_FILE = fullfile(DATASET_DIR, 'SHREC13_SBR_Sketch_Test.cla');
DATASET_CONFIG_MODEL_FILE = fullfile(DATASET_DIR, 'SHREC13_SBR_Model.cla');

RANK_DIR = fullfile(DATASET_DIR, 'RetrievalLists');
RANK_STATS_FILE = fullfile(RANK_DIR, 'Stats_test.txt');
RANK_PRC_FILE = fullfile(RANK_DIR, 'PR_test.txt');

addpath('util');
addpath(DATASET_DIR);

[queryClass, ~] = read_classification_file_shrec13(DATASET_CONFIG_SKETCH_FILE);
[targetClass, N] = read_classification_file_shrec13(DATASET_CONFIG_MODEL_FILE);
targetClass(:, 1) = strcat('m', targetClass(:, 1));

%% Evaluate retrieval performance.
fprintf('Evaluating Rank Lists: %s.\n', RANK_DIR);

numQueries = size(queryClass, 1);
numTargets = size(targetClass, 1);
numClasses = size(N, 1);
  
for i = 1:numQueries 
    for j = 1:numClasses
        if (strcmp(queryClass{i, 2}, N{j, 1}) == 1)
            C(i, 1) = N{j, 2};
            break;
        end
    end
end

tic;
[P, AP, NN, FT, ST, DCG, E] = mexEvalRankLists(RANK_DIR, queryClass, targetClass, C);
mAP  = mean(AP); 
mNN  = mean(NN);
mFT  = mean(FT);
mST  = mean(ST);
mDCG = mean(DCG);
mE   = mean(E);
dt = toc;
fprintf('\nAP: %f\nNN: %f\nFT: %f\nST: %f\nDCG: %f\nE: %f\nIn %f minutes.\n', mAP, mNN, mFT, mST, mDCG, mE, dt / 60);

%% Save evaluation result to files.
fid = fopen(RANK_STATS_FILE, 'w');
fprintf(fid, '\t\t NN   \t FT   \t ST   \t E    \t DCG\n');
for i = 1:numQueries
    fprintf(fid, 'No.%d: %2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f\n', i, NN(i), FT(i), ST(i), E(i), DCG(i));
end
fprintf(fid, 'NN   \t FT   \t ST   \t E    \t DCG    \t AP  \n');
fprintf(fid, '%2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f\t %2.3f\n', mNN, mFT, mST, mE, mDCG, mAP);
fclose(fid);

fid = fopen(RANK_PRC_FILE, 'w');
fprintf(fid, 'Recall  \tPrecision\n');
fclose(fid);
Precision = CalAvgPerf(P, C, numQueries, RANK_PRC_FILE);
Recall = [1:20] / 20;

% Draw PR-Curves.
plot(Recall, Precision);
hold on;
plot(Recall, Precision, '*');
axis([0 1 0 1])
hold off;
xlabel('Precision'),ylabel('Recall')
