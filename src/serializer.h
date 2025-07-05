#pragma once
#include <nlohmann/json.hpp>
#include <mutex>
#include <fstream>
#include <filesystem>
#include <array>
#include <lz4.h>

#include <type_traits>
#include <string>
#include <fcntl.h>
//#include <unistd.h>
#
#include <stdexcept>
#include <cstring> 
#define _CRT_SECURE_NO_WARNINGS

using Json = nlohmann::json;

namespace serializer {
	namespace json {
		enum class SaveType {
			FormattedReadable,    // JSON pretty-print
			UnformattedReadable,  // alles in einer Zeile
			Compressed            // gezippter/deflater Output
		};

		static std::mutex mtx;

		template<typename T>
		void saveObject(const T& obj, const std::string& filepath) {
			std::lock_guard<std::mutex> lock(mtx);

			// Erzeuge das JSON aus dem Objekt
			Json j = obj;

			// Schreibe (überschreibe) die Datei
			std::ofstream ofs(filepath, std::ios::trunc);
			if (!ofs.is_open()) {
				throw std::runtime_error("Konnte Datei zum Schreiben nicht öffnen: " + filepath);
			}
			ofs << j.dump(4);  // 4 Leerzeichen Einrückung
		}

		template<typename T>
		T loadObject(const std::string& filepath) {
			std::lock_guard<std::mutex> lock(mtx);

			// Prüfen, ob die Datei existiert
			if (!std::filesystem::exists(filepath)) {
				throw std::runtime_error("Datei nicht gefunden: " + filepath);
			}

			// Datei öffnen und JSON parsen
			std::ifstream ifs(filepath);
			if (!ifs.is_open()) {
				throw std::runtime_error("Konnte Datei nicht öffnen: " + filepath);
			}

			Json j;
			ifs >> j;

			T obj{};
			nlohmann::adl_serializer<T>::from_json(j, obj);
			return obj;
		}

		template<typename T>
		void saveVector(const std::vector<T>& vector, const std::string& filepath) {
			std::lock_guard<std::mutex> lock(mtx);

			// Direkte Konstruktion des JSON-Arrays
			Json j = Json(vector);

			std::ofstream ofs(filepath, std::ios::trunc);
			if (!ofs.is_open())
				throw std::runtime_error("Konnte Datei nicht zum Schreiben öffnen: " + filepath);
			ofs << j.dump(4);
		}

		template<typename T>
		std::vector<T> loadVector(const std::string& filepath) {
			std::lock_guard<std::mutex> lock(mtx);

			if (!std::filesystem::exists(filepath))
				throw std::runtime_error("Datei nicht gefunden: " + filepath);

			std::ifstream ifs(filepath);
			if (!ifs.is_open())
				throw std::runtime_error("Konnte Datei nicht öffnen: " + filepath);

			Json j;
			ifs >> j;
			if (!j.is_array())
				throw std::runtime_error("JSON-Inhalt ist kein Array: " + filepath);

			// j.get<std::vector<T>>() benötigt lediglich, dass T via to_json/from_json definiert ist
			return j.get<std::vector<T>>();
		}
	}
	namespace binary {
		template<typename T>
		class Writer {
			static_assert(std::is_trivially_copyable_v<T>,
				"Only trivially copyable types supported");

		public:
			Writer(const std::string& filepath) {
				std::ios::openmode mode = std::ios::binary;
				mode |= std::ios::trunc;
				ofs_.open(filepath, mode);
				if (!ofs_) {
					throw std::runtime_error("Could not open file: " + filepath);
				}
			}

			~Writer() {
				ofs_.close();
			}

			void save(const T& obj) {
				ofs_.write(reinterpret_cast<const char*>(&obj), sizeof(T));
				if (!ofs_) {
					throw std::runtime_error("Failed to write binary data");
				}
			}

		private:
			std::ofstream ofs_;
		};

		template<typename T>
		void saveObject(const std::string& filepath, const T& obj) {
			static_assert(std::is_trivially_copyable_v<T>, "Only trivially copyable types supported");

			std::ofstream ofs(filepath, std::ios::binary);
			if (!ofs) {
				throw std::runtime_error("Could not open file: " + filepath);
			}

			ofs.write(reinterpret_cast<const char*>(&obj), sizeof(T));
			if (!ofs) {
				throw std::runtime_error("Failed to write binary data to file: " + filepath);
			}
		}
		template<typename T>
		void saveObjectFast(const std::string& filepath, const T& obj) {
			static_assert(std::is_trivially_copyable_v<T>,
				"Only trivially copyable types supported");

			// 1) Datei öffnen (MSVC-sicher)
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.c_str(), "wb");
			if (err || !f) throw std::runtime_error("fopen_s failed");
#else
			f = std::fopen(filepath.c_str(), "wb");
			if (!f) throw std::runtime_error("fopen failed");
#endif

			// 2) Großer Puffer (z.B. 1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// 3) Einmaliges Block-Write
			if (std::fwrite(&obj, sizeof(T), 1, f) != 1) {
				std::fclose(f);
				throw std::runtime_error("fwrite failed");
			}

			// 4) Schließen
			std::fclose(f);
		}

		template<typename T>
		void saveArray(const std::filesystem::path& filepath, const T* data,
			std::size_t count)
		{
			static_assert(std::is_trivially_copyable_v<T>,
				"saveArrayFast supports only trivially copyable types");

			// --- 1) Datei öffnen (MSVC-sicher)
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.string().c_str(), "wb");
			if (err != 0 || !f) {
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
			}
#else
			f = std::fopen(filepath.string().c_str(), "wb");
			if (!f) {
				throw std::runtime_error("fopen failed");
			}
#endif

			// --- 2) Großer Puffer (1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// --- 3) Anzahl schreiben
			uint64_t n64 = static_cast<uint64_t>(count);
			if (std::fwrite(&n64, sizeof(n64), 1, f) != 1) {
				std::fclose(f);
				throw std::runtime_error("fwrite(count) failed");
			}

			// --- 4) Rohdaten in einem Block
			if (count > 0) {
				if (std::fwrite(data, sizeof(T), count, f) != count) {
					std::fclose(f);
					throw std::runtime_error("fwrite(data) failed");
				}
			}

			// --- 5) Flush & Close
			std::fclose(f);
		}
		template<typename T>
		void saveArrayLZ4(const char* filepath, const T* data, size_t count) {
			const char* src = reinterpret_cast<const char*>(data);
			int srcSize = int(count * sizeof(T));

			// 1) Worst-case Puffer statisch vorhalten
			static thread_local std::vector<char> dst;
			dst.resize(LZ4_compressBound(srcSize));

			int compSize = LZ4_compress_default(src, dst.data(), srcSize, dst.size());
			if (compSize <= 0) throw std::runtime_error("LZ4 failed");

			// 2) Schnell schreiben:
			FILE* f;
			fopen_s(&f, filepath, "wb");
			setvbuf(f, nullptr, _IOFBF, 1 << 20);
			uint64_t rawSize = srcSize, cs = compSize;
			fwrite(&rawSize, sizeof(rawSize), 1, f);
			fwrite(&cs, sizeof(cs), 1, f);
			fwrite(dst.data(), 1, compSize, f);
			fclose(f);
		}

		template<typename T>
		std::vector<T> loadArray(const std::filesystem::path& filepath)
		{
			static_assert(std::is_trivially_copyable_v<T>,
				"loadArrayFast supports only trivially copyable types");

			// --- 1) Datei öffnen (MSVC-sicher)
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.string().c_str(), "rb");
			if (err != 0 || !f) {
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
			}
#else
			f = std::fopen(filepath.string().c_str(), "rb");
			if (!f) {
				throw std::runtime_error("fopen failed");
			}
#endif

			// --- 2) Großer Puffer (1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// --- 3) Anzahl der Elemente lesen
			uint64_t count = 0;
			if (std::fread(&count, sizeof(count), 1, f) != 1) {
				std::fclose(f);
				throw std::runtime_error("fread(count) failed");
			}

			// --- 4) Vektor anlegen
			std::vector<T> vec;
			vec.resize(static_cast<std::size_t>(count));

			// --- 5) Rohdaten in einem Block lesen
			if (count > 0) {
				if (std::fread(vec.data(), sizeof(T), count, f) != count) {
					std::fclose(f);
					throw std::runtime_error("fread(data) failed");
				}
			}

			// --- 6) Close
			std::fclose(f);
			return vec;
		}
		template<typename T>
		void loadArray(const std::filesystem::path& filepath, std::vector<T>& vec)
		{
			static_assert(std::is_trivially_copyable_v<T>,
				"loadArrayFast supports only trivially copyable types");

			// --- 1) Datei öffnen (MSVC-sicher)
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.string().c_str(), "rb");
			if (err != 0 || !f) {
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
			}
#else
			f = std::fopen(filepath.string().c_str(), "rb");
			if (!f) {
				throw std::runtime_error("fopen failed");
			}
#endif

			// --- 2) Großer Puffer (1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// --- 3) Anzahl der Elemente lesen
			uint64_t count = 0;
			if (std::fread(&count, sizeof(count), 1, f) != 1) {
				std::fclose(f);
				throw std::runtime_error("fread(count) failed");
			}

			vec.resize(static_cast<std::size_t>(count));

			// --- 5) Rohdaten in einem Block lesen
			if (count > 0) {
				if (std::fread(vec.data(), sizeof(T), count, f) != count) {
					std::fclose(f);
					throw std::runtime_error("fread(data) failed");
				}
			}

			// --- 6) Close
			std::fclose(f);
		}

		template<typename T>
		std::vector<T> loadArrayLZ4(const std::filesystem::path& filepath) {
			static_assert(std::is_trivially_copyable_v<T>,
				"loadArrayLZ4Fast supports only trivially copyable types");

			// --- 1) Datei öffnen
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.string().c_str(), "rb");
			if (err != 0 || !f)
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
#else
			f = std::fopen(filepath.string().c_str(), "rb");
			if (!f)
				throw std::runtime_error("fopen failed");
#endif

			// --- 2) Großer Puffer für FILE*
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// --- 3) Header lesen
			uint64_t rawSize = 0, compSize = 0;
			if (std::fread(&rawSize, sizeof(rawSize), 1, f) != 1 ||
				std::fread(&compSize, sizeof(compSize), 1, f) != 1)
			{
				std::fclose(f);
				throw std::runtime_error("fread(header) failed");
			}

			// --- 4) Komprimierten Puffer einmalig allokieren
			std::vector<char> compBuf(static_cast<size_t>(compSize));
			if (std::fread(compBuf.data(), 1, compSize, f) != compSize) {
				std::fclose(f);
				throw std::runtime_error("fread(compBuf) failed");
			}

			// --- 5) Close
			std::fclose(f);

			// --- 6) Dekompression in Ziel-Vektor
			size_t count = rawSize / sizeof(T);
			std::vector<T> out(count);
			int res = LZ4_decompress_safe(
				compBuf.data(),
				reinterpret_cast<char*>(out.data()),
				(int)compSize,
				(int)rawSize
			);
			if (res < 0)
				throw std::runtime_error("LZ4 decompression failed");

			return out;
		}
		template<typename T>
		void loadArrayLZ4(const std::filesystem::path& filepath, std::vector<T>& vec) {
			static_assert(std::is_trivially_copyable_v<T>,
				"loadArrayLZ4 supports only trivially copyable types");

			// 1) Datei öffnen
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.string().c_str(), "rb");
			if (err != 0 || !f)
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
#else
			f = std::fopen(filepath.string().c_str(), "rb");
			if (!f)
				throw std::runtime_error("fopen failed");
#endif

			// 2) Großer Puffer (1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// 3) Header lesen
			uint64_t rawSize = 0, compSize = 0;
			if (std::fread(&rawSize, sizeof(rawSize), 1, f) != 1 ||
				std::fread(&compSize, sizeof(compSize), 1, f) != 1)
			{
				std::fclose(f);
				throw std::runtime_error("fread(header) failed");
			}

			// 4) Komprimierten Puffer allokieren
			std::vector<char> compBuf(static_cast<size_t>(compSize));
			if (std::fread(compBuf.data(), 1, compSize, f) != compSize) {
				std::fclose(f);
				throw std::runtime_error("fread(compBuf) failed");
			}

			// 5) Datei schließen
			std::fclose(f);

			// 6) Vektor resize und dekomprimieren
			size_t count = rawSize / sizeof(T);
			vec.resize(count);

			int res = LZ4_decompress_safe(
				compBuf.data(),
				reinterpret_cast<char*>(vec.data()),
				static_cast<int>(compSize),
				static_cast<int>(rawSize)
			);
			if (res < 0) {
				throw std::runtime_error("LZ4 decompression failed");
			}
		}

		template<typename T>
		T loadObject(const std::string& filepath) {
			static_assert(std::is_trivially_copyable_v<T>,
				"Only trivially copyable types supported");

			// Datei im Binärmodus öffnen
			std::ifstream ifs(filepath, std::ios::binary);
			if (!ifs) {
				throw std::runtime_error("Could not open file: " + filepath);
			}

			// Objekt anlegen
			T obj;

			// Bytes in das Objekt einlesen
			ifs.read(reinterpret_cast<char*>(&obj), sizeof(T));
			if (!ifs) {
				throw std::runtime_error("Failed to read binary data from file: " + filepath);
			}

			return obj;
		}
		template<typename T>
		T loadObjectFast(const std::string& filepath) {
			static_assert(std::is_trivially_copyable_v<T>,
				"Only trivially copyable types supported");

			// 1) Datei öffnen (MSVC-sicher)
			FILE* f = nullptr;
#ifdef _MSC_VER
			errno_t err = fopen_s(&f, filepath.c_str(), "rb");
			if (err != 0 || !f) {
				throw std::runtime_error("fopen_s failed with code: " + std::to_string(err));
			}
#else
			f = std::fopen(filepath.c_str(), "rb");
			if (!f) {
				throw std::runtime_error("fopen failed");
			}
#endif

			// 2) Großer Puffer (z.B. 1 MiB)
			static constexpr std::size_t BUF_SZ = 1 << 20;
			static thread_local std::unique_ptr<char[]> buf{ new char[BUF_SZ] };
			std::setvbuf(f, buf.get(), _IOFBF, BUF_SZ);

			// 3) Objekt einlesen
			T obj;
			if (std::fread(&obj, sizeof(T), 1, f) != 1) {
				std::fclose(f);
				throw std::runtime_error("fread failed");
			}

			// 4) Schließen
			std::fclose(f);
			return obj;
		}
	}
}
namespace path {
	enum class Extension {
		Json,
		Txt,
		Bat,
		Cs,
		Html,
		Xml,
		Csv,
		Png,
		Jpeg,
		Pdf,
		Docx
	};
	inline std::string extension_to_string(Extension extension) {
		switch (extension) {
		case Extension::Json: return ".json";
		case Extension::Txt:  return ".txt";
		case Extension::Bat:  return ".bat";
		case Extension::Cs:  return ".cs";
		case Extension::Html:  return ".html";
		case Extension::Xml:  return ".xml";
		case Extension::Csv:  return ".csv";
		case Extension::Png:  return ".png";
		case Extension::Jpeg:  return ".jpeg";
		case Extension::Pdf:  return ".pdf";
		case Extension::Docx:  return ".docx";
		default:               return "";
		}
	}

	template<typename... Parts>
	std::filesystem::path combine(const Parts&... parts) {
		std::vector<std::string> elems{ parts... };
		if (elems.empty()) return {};
		auto& first = elems[0];
		if (first.size() == 2 && std::isalpha(first[0]) && first[1] == ':') {
			first += std::filesystem::path::preferred_separator;
		}
		std::filesystem::path p = first;
		for (size_t i = 1; i < elems.size(); ++i) {
			p /= elems[i];
		}

		return p;
	}
	inline std::string extension(const std::filesystem::path& filepath) {
		return filepath.extension().string();
	}
}
namespace file {
	enum MemoryUnit : size_t {
		Byte = 1,
		KiloByte = 1024,
		MegaByte = 1024 * 1024,
		GigaByte = 1024 * 1024 * 1024,
		TerraByte = 1024 * 1024 * 1024 * 1024
	};

	inline void writeAllBytes(const std::filesystem::path& filepath, const std::vector<uint8_t>& data) {
		std::ofstream ofs(filepath, std::ios::binary | std::ios::trunc);
		if (!ofs.is_open()) {
			throw std::runtime_error("Konnte Datei nicht öffnen zum Schreiben: " + filepath.string());
		}
		ofs.write(reinterpret_cast<const char*>(data.data()), data.size());
		if (!ofs) {
			throw std::runtime_error("Fehler beim Schreiben in die Datei: " + filepath.string());
		}
	}
	inline void writeAllText(const std::filesystem::path& filepath, const std::string& text) {
		std::ofstream ofs(filepath, std::ios::trunc);
		if (!ofs.is_open()) {
			throw std::runtime_error("Konnte Datei nicht öffnen zum Schreiben: " + filepath.string());
		}
		ofs << text;
	}
	inline void writeAllLines(const std::filesystem::path& filepath, const std::vector<std::string>& lines) {
		std::ofstream ofs(filepath, std::ios::trunc);
		if (!ofs.is_open()) {
			throw std::runtime_error("Konnte Datei nicht öffnen zum Schreiben: " + filepath.string());
		}
		for (const auto& line : lines) {
			ofs << line << '\n';
		}
	}
	inline void appendAllText(const std::filesystem::path& filepath, const std::string& text) {
		std::ofstream ofs(filepath, std::ios::app);
		if (!ofs.is_open()) {
			throw std::runtime_error("Konnte Datei nicht öffnen zum Schreiben: " + filepath.string());
		}
		ofs << text;
	}
	inline void appendAllLines(const std::filesystem::path& filepath, const std::vector<std::string>& lines) {
		std::ofstream ofs(filepath, std::ios::app);
		if (!ofs.is_open()) {
			throw std::runtime_error("Konnte Datei nicht öffnen zum Schreiben: " + filepath.string());
		}
		for (const auto& line : lines) {
			ofs << line << '\n';
		}
	}

	bool exists(const std::filesystem::path& filepath) {
		return std::filesystem::exists(filepath);
	}
	inline std::string name(const std::filesystem::path& filepath) {
		return filepath.filename().string();
	}
	inline std::string rename(const std::filesystem::path& filepath, const std::string& newName) {
		std::filesystem::path oldPath(filepath);

		std::string ext = oldPath.extension().string();
		std::filesystem::path newPath = oldPath.parent_path() / (newName + ext);

		std::filesystem::rename(oldPath, newPath);
		return newPath.string();
	}
	inline bool isFile(const std::filesystem::path& filepath) {
		return std::filesystem::is_regular_file(filepath);
	}
	inline double size(const std::filesystem::path& filepath, MemoryUnit unit = MemoryUnit::Byte) {
		const auto bytes = static_cast<double>(std::filesystem::file_size(filepath));
		return bytes / static_cast<double>(unit);
	}
	inline std::vector<std::string> list(const std::filesystem::path& path) {
		std::vector<std::string> files;
		for (auto& entry : std::filesystem::directory_iterator(path)) {
			if (std::filesystem::is_regular_file(entry.status()))
				files.push_back(entry.path().string());
		}
		return files;
	}
}
namespace directory {
	inline void create(const std::filesystem::path& dirpath, bool recursive = true) {
		if (recursive)
			std::filesystem::create_directories(std::filesystem::path(dirpath));
		else
			std::filesystem::create_directory(std::filesystem::path(dirpath));
	}
	inline void clear(const std::filesystem::path& dirpath, bool recursive = false) {
		for (auto& entry : std::filesystem::directory_iterator(dirpath)) {
			if (recursive && std::filesystem::is_directory(entry.path()))
				std::filesystem::remove_all(entry.path());
			else
				std::filesystem::remove(entry.path());
		}
	}
	inline std::vector<std::string> list(const std::filesystem::path& path) {
		std::vector<std::string> dirs;
		std::filesystem::path dir = std::filesystem::path(path);

		for (auto& entry : std::filesystem::directory_iterator(dir)) {
			if (std::filesystem::is_directory(entry.status()))
				dirs.push_back(entry.path().string());
		}
		return dirs;
	}
}
