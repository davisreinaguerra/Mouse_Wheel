function [trigAvg, trigIndiv] = trigAvg(data, time, s_before, s_after, Fs)
trigIndiv = zeros(1+Fs*s_before+Fs*s_after,(size(time,2)-1));

for i = 1:size(time,2)-1
    
    trigIndiv(:,i)=data(time(i)-s_before*Fs : time(i)+s_after*Fs);
    
end

trigAvg=mean(trigIndiv,2);
