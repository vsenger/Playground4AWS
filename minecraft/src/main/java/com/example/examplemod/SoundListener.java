package com.example.examplemod;

import com.amazonaws.services.iot.client.AWSIotMessage;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;
import net.minecraft.server.MinecraftServer;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import org.json.JSONObject;

public class SoundListener extends AWSIotTopic {
	public static String lastNote="";
	// Directly reference a log4j logger.
	private static final Logger LOGGER = LogManager.getLogger();
	private MinecraftServer mcServer = null;
	public SoundListener(String topic, AWSIotQos qos, MinecraftServer mcServer) {
		super(topic, qos);
		this.mcServer = mcServer;
	}

	@Override
	public void onMessage(AWSIotMessage message) {
		lastNote = message.getStringPayload();
		LOGGER.info("Note: " + lastNote);

	}

}
