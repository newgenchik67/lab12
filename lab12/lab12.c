#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#define COUNT 5 
#define MINUTES_PER_HOUR 60
#define HOURS_PER_DAY 24
#define FILENAME "flights.txt"
struct Time {
	int hours;
	int minutes;
};
struct Flight {
	char flight_number[20];
	struct Time departure;
	struct Time arrival;
	char destination[50];
	char aircraft[40];
	double distance;
};
double calculate_duration_hours(struct Time dep, struct Time arr);
double calculate_speed(struct Flight f, double duration_hours);
void print_flight(struct Flight f, double duration_hours, double speed);
void find_fastest_flight(struct Flight flights[], int count);
int writefile(const char* fname, struct Flight* flights, int size);
int main(void) {
	struct Flight flights[COUNT];
	int i;
	setlocale(LC_ALL, "RUS");
	printf("Ввод данных об авиарейсах (всего записей: %d)\n\n", COUNT);
	for (i = 0; i < COUNT; i++) {
		printf("--- Авиарейс %d ---\n", i + 1);
		printf("№ авиарейса (без пробелов): ");
		scanf("%19s", flights[i].flight_number);
		printf("Время вылета (часы минуты): ");
		scanf("%d %d", &flights[i].departure.hours, &flights[i].departure.minutes);
		printf("Время прилета (часы минуты): ");
		scanf("%d %d", &flights[i].arrival.hours, &flights[i].arrival.minutes);
		printf("Направление (без пробелов, напр. Москва-Сочи): ");
		scanf("%49s", flights[i].destination);
		printf("Марка самолета (без пробелов, напр. Boeing-737): ");
		scanf("%39s", flights[i].aircraft);
		printf("Расстояние (км): ");
		scanf("%lf", &flights[i].distance);
		printf("\n");
	}
	find_fastest_flight(flights, COUNT);
	if (writefile(FILENAME, flights, COUNT)) {
		printf("\nДанные успешно записаны в файл %s\n", FILENAME);
	}
	else {
		printf("\nОшибка при записи данных в файл %s\n", FILENAME);
		return 1;
	}
	return 0;
}
double calculate_duration_hours(struct Time dep, struct Time arr) {
	int dep_min = dep.hours * MINUTES_PER_HOUR + dep.minutes;
	int arr_min = arr.hours * MINUTES_PER_HOUR + arr.minutes;
	int diff = arr_min - dep_min;
	if (diff < 0) {
		diff += HOURS_PER_DAY * MINUTES_PER_HOUR;
	}
	return (double)diff / MINUTES_PER_HOUR;
}
double calculate_speed(struct Flight f, double duration_hours) {
	if (duration_hours <= 0.0) {
		return 0.0;
	}
	return f.distance / duration_hours;
}
void print_flight(struct Flight f, double duration_hours, double speed) {
	printf("№ авиарейса:      %s\n", f.flight_number);
	printf("Время вылета:     %02d:%02d\n", f.departure.hours, f.departure.minutes);
	printf("Время прилета:    %02d:%02d\n", f.arrival.hours, f.arrival.minutes);
	printf("Направление:      %s\n", f.destination);
	printf("Марка самолета:   %s\n", f.aircraft);
	printf("Расстояние:       %.2lf км\n", f.distance);
	printf("Время в пути:     %.2lf ч\n", duration_hours);
	printf("Средняя скорость: %.2lf км/ч\n", speed);
}
void find_fastest_flight(struct Flight flights[], int count) {
	int i;
	int max_index = 0;
	double duration, speed, max_speed;
	max_speed = calculate_speed(flights[0], calculate_duration_hours(flights[0].departure, flights[0].arrival));
	for (i = 1; i < count; i++) {
		duration = calculate_duration_hours(flights[i].departure, flights[i].arrival);
		speed = calculate_speed(flights[i], duration);
		if (speed > max_speed) {
			max_speed = speed;
			max_index = i;
		}
	}
	printf("=== Авиарейс с максимальной скоростью ===\n");
	duration = calculate_duration_hours(flights[max_index].departure, flights[max_index].arrival);
	print_flight(flights[max_index], duration, max_speed);
}
int writefile(const char* fname, struct Flight* flights, int size) {
	FILE* out;
	int i;
	double duration, speed;
	if ((out = fopen(fname, "w")) == NULL) {
		printf("Ошибка открытия файла для записи\n");
		return 0;
	}
	for (i = 0; i < size; i++) {
		duration = calculate_duration_hours(flights[i].departure, flights[i].arrival);
		speed = calculate_speed(flights[i], duration);
		if (fprintf(out, "%s %02d %02d %02d %02d %s %s %.2lf %.2lf %.2lf\n",
			flights[i].flight_number,
			flights[i].departure.hours, flights[i].departure.minutes,
			flights[i].arrival.hours, flights[i].arrival.minutes,
			flights[i].destination,
			flights[i].aircraft,
			flights[i].distance,
			duration,
			speed) < 0) {
			printf("Ошибка записи данных в файл\n");
			fclose(out);
			return 0;
		}
	}
	if (fclose(out) == EOF) {
		printf("Ошибка закрытия файла\n");
		return 0;
	}
	return 1;
}