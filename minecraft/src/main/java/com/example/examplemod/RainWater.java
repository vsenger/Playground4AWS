package com.example.examplemod;
import net.minecraft.entity.Entity;
//import net.minecraft.init.Blocks;
import net.minecraft.world.World;
import net.minecraftforge.event.entity.living.LivingEvent.LivingUpdateEvent;
//import net.minecraftforge.fml.common.eventhandler.SubscribeEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import java.util.Random;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

public class RainWater {
    private static final Logger LOGGER = LogManager.getLogger();
	private static final double HIGH_HUMIDITY = 75;
    @SubscribeEvent
    public void makeWater(LivingUpdateEvent event) {
        Entity entity = event.getEntity();
		try {
			World world = entity.getCommandSenderWorld();
			if(SensorsListener.lastHumidity>HIGH_HUMIDITY ||
					SoundListener.lastNote.equals("g")) {
				world.setRainLevel(new Random().nextFloat());
				world.setThunderLevel(new Random().nextFloat());
			}
			else {
				world.setRainLevel(0);
				world.setThunderLevel(0);
			}
		} catch (Exception e) {
			LOGGER.error("Turning rain on", e);
		}
  	}
}