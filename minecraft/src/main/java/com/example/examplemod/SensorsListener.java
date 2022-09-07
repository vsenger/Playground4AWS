package com.example.examplemod;

import com.amazonaws.services.iot.client.AWSIotMessage;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;
import net.minecraft.server.MinecraftServer;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import org.json.JSONObject;
public class SensorsListener extends AWSIotTopic {
	public static double lastHumidity;
	public static double lastTemperature;
	// Directly reference a log4j logger.
	private static final Logger LOGGER = LogManager.getLogger();
	private MinecraftServer mcServer = null;
	private String lastMessage;
	public SensorsListener(String topic, AWSIotQos qos, MinecraftServer mcServer) {
		super(topic, qos);
		this.mcServer = mcServer;
	}

	@Override
	public void onMessage(AWSIotMessage message) {
		//LOGGER.info(System.currentTimeMillis() + ": <<< " + message.getStringPayload());
		lastMessage = message.getStringPayload();
		//LOGGER.info("humidity... ");

		JSONObject tomJsonObject = new JSONObject(lastMessage);
		LOGGER.info(tomJsonObject);
		lastHumidity = tomJsonObject.getDouble("humidity");
		lastTemperature = tomJsonObject.getDouble("temperature");
		LOGGER.info("humidity: " + tomJsonObject.getDouble("humidity"));
	}

}
