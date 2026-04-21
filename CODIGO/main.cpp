
#include <iostream>
#include <ctime>
#include <fstream>
#include <windows.h>

// Constantes
const int FILAS = 17;
const int COLUMNAS = 17;
const int MAX_USERS = 100;
const int ANCHO = 45;

// Tarifas
const int TARIFA_CARRO = 100;
const int TARIFA_MOTO = 50;

// Usuarios
std::string usuarios[MAX_USERS];
std::string contras[MAX_USERS];
int totalUsuarios = 0;

// Calificaciones
int sumaCalificaciones = 0;
int totalCalificaciones = 0;

// Tipos
enum TipoVehiculo { CARRO, MOTO };

// Estructura
struct Vehiculo {
    std::string placa;
    time_t horaEntrada;
    bool ocupado;
    TipoVehiculo tipo;
};

// ===== PROTOTIPOS =====
void menuAdmin(char p[FILAS][COLUMNAS], Vehiculo* v);
void menuUsuario(char p[FILAS][COLUMNAS], Vehiculo* v);
void menuInicial(char p[FILAS][COLUMNAS], Vehiculo* v);

// ===== UTIL =====
void limpiarEntrada() {
    if (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
    }
}

// ===== VISUAL =====
void colorAzulBlanco() {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 31);
}

void imprimirCentrado(std::string texto) {
    int espacios = (ANCHO - texto.length()) / 2;
    for (int i = 0; i < espacios; i++) std::cout << " ";
    std::cout << texto << std::endl;
}

void linea() {
    std::cout << "---------------------------------------------\n";
}

// ================= TARIFAS =================
void mostrarTarifas() {
    linea();
    imprimirCentrado("TARIFAS DEL PARQUEADERO");
    linea();

    imprimirCentrado("Carro: $" + std::to_string(TARIFA_CARRO) + " por minuto");
    imprimirCentrado("Moto: $" + std::to_string(TARIFA_MOTO) + " por minuto");

    linea();
    imprimirCentrado("Si calificas >= 3, obtienes 10% de descuento");

    if (totalCalificaciones > 0) {
        double promedio = (double)sumaCalificaciones / totalCalificaciones;
        imprimirCentrado("Promedio: " + std::to_string(promedio));
    } else {
        imprimirCentrado("Sin calificaciones.");
    }

    linea();
}

// ================= HISTORIAL =================
void guardarHistorial(std::string placa, int pago, int calificacion) {
    std::ofstream archivo("historial.txt", std::ios::app);
    archivo << "Placa: " << placa
            << " | Pago: $" << pago
            << " | Calificacion: " << calificacion << "\n";
}

void mostrarHistorial() {
    std::ifstream archivo("historial.txt");

    linea();
    imprimirCentrado("HISTORIAL");
    linea();

    std::string lineaArchivo;

    if (!archivo) {
        imprimirCentrado("No hay historial.");
    } else {
        while (std::getline(archivo, lineaArchivo)) {
            std::cout << lineaArchivo << std::endl;
        }
    }

    linea();
}

// ================= USUARIOS =================
void cargarUsuarios() {
    std::ifstream archivo("usuarios.txt");

    if (!archivo) {
        usuarios[0] = "admin";
        contras[0] = "1234";
        totalUsuarios = 1;
        return;
    }

    while (archivo >> usuarios[totalUsuarios] >> contras[totalUsuarios]) {
        totalUsuarios++;
        if (totalUsuarios >= MAX_USERS) break;
    }
}

void guardarUsuario(std::string user, std::string pass) {
    std::ofstream archivo("usuarios.txt", std::ios::app);
    archivo << user << " " << pass << "\n";
}

void crearCuenta() {
    if (totalUsuarios >= MAX_USERS) {
        std::cout << "Limite alcanzado\n";
        return;
    }

    std::string user, pass;

    std::cout << "Usuario: ";
    std::cin >> user;

    std::cout << "Contraseña: ";
    std::cin >> pass;

    usuarios[totalUsuarios] = user;
    contras[totalUsuarios] = pass;
    totalUsuarios++;

    guardarUsuario(user, pass);
    std::cout << "Cuenta creada\n";
}

bool login() {
    std::string user, pass;

    std::cout << "Usuario: ";
    std::cin >> user;

    std::cout << "Contraseña: ";
    std::cin >> pass;

    for (int i = 0; i < totalUsuarios; i++) {
        if (user == usuarios[i] && pass == contras[i])
            return true;
    }

    std::cout << "Error\n";
    return false;
}

// ================= PARQUEADERO =================
void inicializar(char p[FILAS][COLUMNAS], Vehiculo* v) {
    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (i == 0 || i == FILAS - 1 || j == 0 || j == COLUMNAS - 1)
                p[i][j] = '#';
            else if (i == 1 && j == 1)
                p[i][j] = 'E';
            else if (i % 2 == 0)
                p[i][j] = '-';
            else
                p[i][j] = ' ';
        }
    }

    p[FILAS - 2][COLUMNAS - 2] = 'S';

    for (int k = 0; k < FILAS * COLUMNAS; k++)
        v[k].ocupado = false;
}

// ================= FUNCIONES =================
int calcularPago(Vehiculo v) {
    time_t salida = time(0);
    double segundos = difftime(salida, v.horaEntrada);

    int minutos = (int)(segundos / 60);
    if (segundos > 0) minutos++;

    int tarifa = (v.tipo == CARRO) ? TARIFA_CARRO : TARIFA_MOTO;
    return minutos * tarifa;
}

void mostrarMapa(char p[FILAS][COLUMNAS]) {
    int libres = 0;
    int ocupados = 0;

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (p[i][j] == '-') libres++;
            if (p[i][j] == 'O') ocupados++;
        }
    }

    linea();
    imprimirCentrado("PARQUEADERO");
    linea();

    imprimirCentrado("Libres: " + std::to_string(libres));
    imprimirCentrado("Ocupados: " + std::to_string(ocupados));
    linea();

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++)
            std::cout << p[i][j] << " ";
        std::cout << std::endl;
    }

    linea();
}

void ingresarVehiculo(char p[FILAS][COLUMNAS], Vehiculo* v) {
    std::string placa;
    int tipo;

    std::cout << "Placa: ";
    std::cin >> placa;

    std::cout << "Tipo (0=Carro,1=Moto): ";
    std::cin >> tipo;

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            if (p[i][j] == '-') {
                p[i][j] = 'O';

                int pos = i * COLUMNAS + j;
                v[pos].placa = placa;
                v[pos].horaEntrada = time(0);
                v[pos].ocupado = true;
                v[pos].tipo = (tipo == 0) ? CARRO : MOTO;
                return;
            }
        }
    }

    std::cout << "Lleno\n";
}

void retirarVehiculo(char p[FILAS][COLUMNAS], Vehiculo* v) {
    std::string placa;
    std::cout << "Placa: ";
    std::cin >> placa;

    for (int i = 0; i < FILAS; i++) {
        for (int j = 0; j < COLUMNAS; j++) {
            int pos = i * COLUMNAS + j;

            if (v[pos].ocupado && v[pos].placa == placa) {
                int pago = calcularPago(v[pos]);

                int cal;
                std::cout << "Califique (1-5): ";
                std::cin >> cal;

                sumaCalificaciones += cal;
                totalCalificaciones++;

                if (cal >= 3) pago *= 0.9;

                guardarHistorial(placa, pago, cal);

                p[i][j] = '-';
                v[pos].ocupado = false;

                std::cout << "Total: $" << pago << std::endl;
                return;
            }
        }
    }

    std::cout << "No encontrado\n";
}

// ================= MENUS =================
void menuAdmin(char p[FILAS][COLUMNAS], Vehiculo* v) {
    int op;
    do {
        std::cout << "\n1.Ingresar 2.Mapa 3.Retirar 4.Historial 0.Salir\n";
        std::cin >> op;

        if (op == 1) ingresarVehiculo(p, v);
        if (op == 2) mostrarMapa(p);
        if (op == 3) retirarVehiculo(p, v);
        if (op == 4) mostrarHistorial();

    } while (op != 0);
}

void menuUsuario(char p[FILAS][COLUMNAS], Vehiculo* v) {
    int op;
    do {
        std::cout << "\n1.Mapa 2.Retirar 0.Salir\n";
        std::cin >> op;

        if (op == 1) mostrarMapa(p);
        if (op == 2) retirarVehiculo(p, v);

    } while (op != 0);
}

void menuInicial(char p[FILAS][COLUMNAS], Vehiculo* v) {
    int op;
    do {
        std::cout << "\n1.Admin 2.Usuario 3.Crear 4.Tarifas 0.Salir\n";
        std::cin >> op;

        if (op == 1 && login()) menuAdmin(p, v);
        if (op == 2) menuUsuario(p, v);
        if (op == 3) crearCuenta();
        if (op == 4) mostrarTarifas();

    } while (op != 0);
}

// ================= MAIN =================
int main() {
    char parqueadero[FILAS][COLUMNAS];
    Vehiculo vehiculos[FILAS * COLUMNAS];

    cargarUsuarios();
    inicializar(parqueadero, vehiculos);
    menuInicial(parqueadero, vehiculos);

    return 0;
}