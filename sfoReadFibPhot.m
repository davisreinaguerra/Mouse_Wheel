function [spectTimes, filtSig, sig, spectFreqs, spectAmpVals] = ...
    sfoReadFibPhot(rawData, fs, freqRange, params)

% params.winSize2 = 1;
% params.spectSample2 = params.winSize2 ./ 5;

% Reads in:
% 1) rawData -- the raw oscillating photometry signal
% 2) fs -- sampling frequency
% 3) freqRange -- 2 element array of frequency ranges to be analyzed
% 4) params -- structure with following elements (and default values used):

% Scott Owen -- 2018-08-12

% Convert spectrogram window size and overlap from time to samples
spectWindow = 2.^nextpow2(fs .* params.winSize);
spectOverlap = ceil(spectWindow - (spectWindow .* (params.spectSample ./ params.winSize)));
disp(['Spectrum window ', num2str(spectWindow ./ fs), ' sec; ',...
    num2str(spectWindow), ' samples at ', num2str(fs), ' Hz'])

% Create low pass filter for final data
lpFilt = designfilt('lowpassiir','FilterOrder',8, 'PassbandFrequency',params.filtCut,...
    'PassbandRipple',0.01, 'SampleRate',fs);

% Calculate spectrogram
[spectVals,spectFreqs,spectTimes]=spectrogram(rawData,spectWindow,spectOverlap,freqRange,fs);
% Convert spectrogram to real units
spectAmpVals = double(abs(spectVals)); 
% Find the two carrier frequencies
avgFreqAmps = mean(spectAmpVals,2);
% [pks,locs]=findpeaks(double(avgFreqAmps),'minpeakheight',max(avgFreqAmps./10));
[pks,locs]=findpeaks(double(avgFreqAmps));
if max(pks) == 1 && length(pks) > 1
    pks(1) = []; locs(1) = [];
end
[maxVal,maxFreqBin] = max(pks);
maxFreqBin = locs(maxFreqBin);
% Calculate signal at each frequency band
sig = mean(abs(spectVals((maxFreqBin-params.inclFreqWin):(maxFreqBin+params.inclFreqWin),:)),1);
% Low pass filter the signals
filtSig = filtfilt(lpFilt,double(sig));
