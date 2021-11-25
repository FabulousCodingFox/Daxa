#include "DaxaCore.hpp"

#include "rendering/Renderer.hpp"
#include "platform/Window.hpp"

namespace daxa {

	struct AppState {
		bool continueRunning = true;
	};

	class User {
	public:
		virtual void init(std::shared_ptr<AppState>& appstate)		= 0;
		virtual void update(std::shared_ptr<AppState>& appstate)	= 0;
		virtual void deinit(std::shared_ptr<AppState>& appstate)	= 0;
	};

	class Application {
	public:
		Application(u32 winWidth, u32 winHeight, std::string const& winName, std::unique_ptr<User> user);
		void run();
	private:
		// the reason behind the exessive use of shared ptr here is for later multithreadding use.
		// the lifetimes in multithreadding can get very complex so shared_ptrs are appropriate for and ONLY FOR these "low frequency" types wich are accessed and copied rarely.
		std::shared_ptr<AppState> appstate;
		std::shared_ptr<Window> window;
		std::shared_ptr<Renderer> renderer;
		std::unique_ptr<User> user;
	};

}
