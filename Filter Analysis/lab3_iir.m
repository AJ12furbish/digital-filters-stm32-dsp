[h,f] = freqz(Num,1,2400,48000);

dB = @(x) 20*log10(abs(x));

freq = f(101:100:2001);
resp_dB = dB(h(101:100:2001));
table(freq,resp_dB)

xlswrite('theoretical_response_iir.xlsx',[freq,resp_dB]);

%% 
sprintf('%f,',Num)
sprintf('%f,',Den)

%% 

sprintf('{%f,%f,%f},\n',SOS(1,1:3),SOS(2,1:3),SOS(3,1:3))
sprintf('{%f,%f,%f},\n',SOS(1,4:6),SOS(2,4:6),SOS(3,4:6))
sprintf('%f,',G)