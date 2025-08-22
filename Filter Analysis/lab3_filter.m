[h,f] = freqz(Num1,1,2400,48000);

dB = @(x) 20*log10(abs(x));

freq = f(101:100:2001);
resp_dB = dB(h(101:100:2001));
table(freq,resp_dB);

%%
xlswrite('theoretical_response.xlsx',[freq,resp_dB]);

%%
sprintf('%f,',Num1)