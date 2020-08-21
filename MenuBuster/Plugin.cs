using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using IPA;
using IPA.Config;
using IPA.Config.Stores;
using UnityEngine.SceneManagement;
using UnityEngine;
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
		
		#region BSIPA Config
		//Uncomment to use BSIPA's config
		/*
		[Init]
		public void InitWithConfig(Config conf)
		{
			Configuration.PluginConfig.Instance = conf.Generated<Configuration.PluginConfig>();
			Logger.log.Debug("Config loaded");
		}
		*/
		#endregion
		
		public void OnSceneChanged(Scene prevScene, Scene nextScene) {
			
			if (nextScene.name == "GameCore") {
				
				Logger.log.Info("Disabling system button");
				
				//Disable system button
				
			} else if (prevScene.name == "GameCore") {
				
				Logger.log.Info("Reenabling system button");
				
				//Reenable system button
				
			}
			
		}
		
		[OnStart]
		public void OnApplicationStart() {
			
			Server.StartServer();
			
			bool success = DLLInjector.InjectDLL("vrserver", Directory.GetCurrentDirectory() + "\\Libs\\MenuBusterCore.dll");
			
			if (success)
				Logger.log.Info("Successfully injected MenuBusterCore");
			else
				Logger.log.Error("Couldn't inject MenuBusterCore");

			new GameObject("MenuBusterController").AddComponent<MenuBusterController>();
			
		}
		
		[OnExit]
		public void OnApplicationQuit() {
			
			Server.Exit();
			
			Logger.log.Debug("OnApplicationQuit");
			
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