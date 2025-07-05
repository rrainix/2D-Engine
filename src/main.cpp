#include <glad/glad.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <iostream>
#include "EngineMain.h"
#include "Experimental/FlappyBirdMainSystem.h"

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 12345;
constexpr int BUF_SIZE = 1024;

void parseCommand(const std::string& input) {
	std::istringstream iss(input);
	std::string command;
	if (!(iss >> command)) {
		// leere Eingabe
		return;
	}

	engine::Scene& scene = engine::SceneManager::getActiveScene();

	if (command == "spawn") {
		int x, y;
		if (!(iss >> x >> y)) {
			std::cout << "[ERROR] spawn benötigt zwei Zahlen: spawn x y\n";
			return;
		}

		std::cout << "[INFO] Spawne Entity bei Position: " << x << ", " << y << "\n";

		entt::entity handle = scene.createEntity();
		scene.addComponent<graphics::SpriteRenderer>(handle);
		scene.addComponent<engine::Transform2D>(handle,
			engine::Transform2D::FromPosition(glm::vec2{ float(x), float(y) }));
		scene.addComponent<engine::Rigidbody2D>(handle);
		scene.addComponent<engine::BoxCollider>(handle);
	}
	else if (command == "test") {
		int amount;
		if (!(iss >> amount)) {
			std::cout << "[ERROR] test benötigt eine Zahl: test <Anzahl>\n";
			return;
		}

		std::cout << "[INFO] Erzeuge " << amount << " Test‑Entities\n";
		for (int i = 0; i < amount; i++) {
			entt::entity handle = scene.createEntity();
			scene.addComponent<graphics::SpriteRenderer>(handle);
			scene.addComponent<engine::Transform2D>(handle,
				engine::Transform2D::FromPosition(glm::vec2{ 0.0f, 5.0f }));
			scene.addComponent<engine::Rigidbody2D>(handle);
			scene.addComponent<engine::BoxCollider>(handle);
		}
	}
	else {
		std::cout << "[WARNUNG] Unbekannter Befehl: " << command << "\n";
	}
}

// Hier kannst du einfache Befehle empfangen (z. B. „spawn x y“)
void HandleClient(SOCKET clientSocket)
{
	char buf[BUF_SIZE];
	while (true)
	{
		int n = recv(clientSocket, buf, BUF_SIZE - 1, 0);
		if (n <= 0) break;

		buf[n] = '\0';
		std::string command(buf);
		parseCommand(command);


		// Beispiel: Hier könntest du jetzt den Befehl analysieren und in der Engine weitergeben
		// z.B. if (command.starts_with("spawn")) { Engine::Spawn(...); }
	}

	closesocket(clientSocket);
}

// Diese Funktion wird beim Start aufgerufen, um den Host zu starten
void StartNetworkHost()
{
	std::thread([]() {
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			std::cerr << "WSAStartup failed\n";
			return;
		}

		SOCKET listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (listenSock == INVALID_SOCKET)
		{
			std::cerr << "Socket creation failed\n";
			WSACleanup();
			return;
		}

		sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = INADDR_ANY;
		serverAddr.sin_port = htons(PORT);

		if (bind(listenSock, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) == SOCKET_ERROR ||
			listen(listenSock, SOMAXCONN) == SOCKET_ERROR)
		{
			std::cerr << "Bind/Listen failed\n";
			closesocket(listenSock);
			WSACleanup();
			return;
		}

		std::cout << "[Engine Host] Warte auf Verbindungen auf Port " << PORT << "...\n";

		while (true)
		{
			SOCKET clientSocket = accept(listenSock, nullptr, nullptr);
			if (clientSocket != INVALID_SOCKET)
			{
				std::cout << "[Engine Host] Client verbunden!\n";
				std::thread(HandleClient, clientSocket).detach();
			}
		}

		closesocket(listenSock);
		WSACleanup();
		}).detach(); // detach, damit die Engine direkt weitermacht
}



int main() {
	Window::init();
	Window appWindow{ 800, 800, "2D Engine", engine::WindowMode::WINDOWED_MAXIMIZED };
	Window::vsync(false);
	gladLoadGL();



	try
	{
		Application application{ appWindow };
		TextureManager::loadTexture("pipe-green.png", FilterMode::None);
		TextureManager::loadTexture("bluebird-downflap.png", FilterMode::None);
		TextureManager::loadTexture("bluebird-midflap.png", FilterMode::None);
		TextureManager::loadTexture("bluebird-upflap.png", FilterMode::None);

		const std::string name = "Flappy Bird";
		SceneManager::createScene(name);
		Scene& scene = SceneManager::loadScene(name);
		scene.addSystem<GizmosRenderSystem>();
		scene.addSystem<FlappyBirdMainSystem>();
		application.run();
	}
	catch (std::runtime_error e)
	{
		std::cout << e.what() << '\n';
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}