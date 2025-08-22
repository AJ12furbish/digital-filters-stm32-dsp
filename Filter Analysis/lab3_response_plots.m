measured_response=readtable("measured_response.xlsx");
theoretical_response=readtable("theoretical_response.xlsx");


figure;
hold on;
plot(measured_response(1:20,1), measured_response(1:20, 2), 'DisplayName', "Measured", "Color", "red");
plot(theoretical_response(1:20,1), theoretical_response(1:20, 2), 'DisplayName', "Theoretical", "Color", "blue");
xlabel("Frequency [Hz]");
ylabel("Amplitude [dB]");
grid on;
legend("Visible","on");
hold off;