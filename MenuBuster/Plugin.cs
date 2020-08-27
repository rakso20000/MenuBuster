using IPA;
using UnityEngine.SceneManagement;
using IPALogger = IPA.Logging.Logger;
using System.IO;

namespace MenuBuster {
	
	[Plugin(RuntimeOptions.SingleStartInit)]
	public class Plugin {
		internal static Plugin instance { get; private set; }
		internal static string Name => "MenuBuster";
		
		[Init]
		/// <summary>
		/// Called when the plugin is first loaded by IPA (either when the game starts or when the plugin is enabled if it starts disabled).
		/// [Init] methods that use a Constructor or called before regular methods like InitWithConfig.
		/// Only use [Init] with one Constructor.
		/// </summary>
		public void Init(IPALogger logger) {
			
			instance = this;
			Logger.log = logger;
			Logger.log.Debug("Logger initialized.");
			
		}
		
		public void OnSceneChanged(Scene prevScene, Scene nextScene) {
			
			if (nextScene.name == "GameCore") {
				
				Logger.log.Info("Disabling system button");
				
				Server.SetInSong(true);
				
			} else if (prevScene.name == "GameCore") {
				
				Logger.log.Info("Reenabling system button");
				
				Server.SetInSong(false);
				
			}
			
		}
		
		[OnStart]
		public void OnApplicationStart() {
			
			Server.StartServer();
			
			bool success = DLLInjector.InjectDLL("vrserver", IPA.Utilities.UnityGame.NativeLibraryPath + "\\MenuBusterCore.dll");
			
			if (success)
				Logger.log.Info("Successfully injected MenuBusterCore");
			else
				Logger.log.Error("Couldn't inject MenuBusterCore");
			
		}
		
		[OnExit]
		public void OnApplicationQuit() {
			
			Server.Exit();
			
		}
		
		[OnEnable]
		public void OnEnable() {
			
			SceneManager.activeSceneChanged += OnSceneChanged;
			
		}
		
		[OnDisable]
		public void OnDisable() {
			
			SceneManager.activeSceneChanged -= OnSceneChanged;
			
		}
		
	}
	
}