%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%	The precision at a specified recall level will be 
%	bilinearly interpolated from neighboring precision values.
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [v]=InterpolatePerf(values, num, index)
    bins = num;

    if(bins==0)
        v=0;
    else
        xx=index*bins;
        x1=fix(xx);
        x2=x1+1;
        dx=xx-x1;
        if(x1<1)
            x1=1;
        end
        if(x2<1)
            x2=1;
        end
        if(x1>bins)
            x1=bins;
        end
        if(x2>bins)
            x2=bins;
        end
        
        v=values(x1)*(1-dx)+values(x2)*dx;  
    end