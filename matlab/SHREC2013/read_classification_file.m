%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% read a sketch/model classification file (models' name and class, class information).
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [modelclass, N] = read_classification_file(filename)

fp=fopen(filename,'r');
fscanf(fp,'%s',2);
classnum=fscanf(fp, '%d',1);
modelnum=fscanf(fp, '%d',1);
modelclass=cell(modelnum,2);
count=1;
N=cell(classnum,2);
for i=1:classnum
    classname=fscanf(fp,'%s',1);
    parentclassname=fscanf(fp,'%s',1);
    model_num_class=fscanf(fp,'%d',1);
    N{i,1}=classname;
    N{i,2}=model_num_class;    
    for j=1:model_num_class
        model_index=fscanf(fp,'%s',1);
        modelclass{count,1}=model_index;
        modelclass{count,2}=classname;
        count=count+1;
    end
end
fclose(fp);