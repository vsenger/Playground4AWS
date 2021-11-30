package com.example.examplemod;

import com.amazonaws.services.iot.client.AWSIotMqttClient;
import com.amazonaws.services.iot.client.AWSIotQos;
import com.amazonaws.services.iot.client.AWSIotTopic;
import com.amazonaws.services.iot.client.sample.sampleUtil.SampleUtil;
import net.minecraft.block.Block;
import net.minecraft.block.Blocks;
import net.minecraft.server.MinecraftServer;
import net.minecraft.world.World;
import net.minecraftforge.common.MinecraftForge;
import net.minecraftforge.event.RegistryEvent;
import net.minecraftforge.event.entity.living.LivingEvent;
import net.minecraftforge.event.world.BlockEvent;
import net.minecraftforge.eventbus.api.Event;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import net.minecraftforge.fml.InterModComms;
import net.minecraftforge.fml.common.Mod;
import net.minecraftforge.fml.event.lifecycle.FMLClientSetupEvent;
import net.minecraftforge.fml.event.lifecycle.FMLCommonSetupEvent;
import net.minecraftforge.fml.event.lifecycle.InterModEnqueueEvent;
import net.minecraftforge.fml.event.lifecycle.InterModProcessEvent;
import net.minecraftforge.fml.event.server.FMLServerStartingEvent;
import net.minecraftforge.fml.javafmlmod.FMLJavaModLoadingContext;
import net.minecraftforge.fml.server.ServerLifecycleHooks;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

import java.util.Random;
import java.util.stream.Collectors;


// The value here should match an entry in the META-INF/mods.toml file
@Mod("examplemod")
public class ExampleMod
{
	// Directly reference a log4j logger.
    private static final Logger LOGGER = LogManager.getLogger();

	public static final String AWS_IOT_TOPIC = "control/lamp";
	// public static final String AWS_IOT_ENDPOINT = "<prefix>-ats.iot.<region>.amazonaws.com";
	public static final String AWS_IOT_ENDPOINT = "a2p4fyajwx9lux-ats.iot.us-east-1.amazonaws.com";
	public static final String AWS_IOT_CLIENT_ID = "jump-mod";
	public static final String AWS_IOT_CERTIFICATE_FILE = "/Users/vsenger/minecraft-iot/8f2b2f776911332a0fea819064421830e592b55f32c4a6262918700841fc5c32-certificate.pem.crt";
	public static final String AWS_IOT_PRIVATE_KEY_FILE = "/Users/vsenger/minecraft-iot/8f2b2f776911332a0fea819064421830e592b55f32c4a6262918700841fc5c32-private.pem.key";

	private static final String AWS_IOT_INBOUND_TOPIC = "playground/sensors";
	private static final AWSIotQos AWS_IOT_INBOUND_TOPIC_QOS = AWSIotQos.QOS0;

	AWSIotMqttClient client = null;

	public ExampleMod() {
        // Register the setup method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::setup);
        // Register the enqueueIMC method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::enqueueIMC);
        // Register the processIMC method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::processIMC);
        // Register the doClientStuff method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::doClientStuff);

        // Register ourselves for server and other game events we are interested in
        MinecraftForge.EVENT_BUS.register(this);
		MinecraftForge.EVENT_BUS.register(new RainWater());
    }

    private void setup(final FMLCommonSetupEvent event)
    {
        // some preinit code
        LOGGER.info("HELLO FROM PREINIT");
        LOGGER.info("DIRT BLOCK >> {}", Blocks.DIRT.getRegistryName());

		String clientEndpoint = AWS_IOT_ENDPOINT;
		String clientId = AWS_IOT_CLIENT_ID;
		String certificateFile = AWS_IOT_CERTIFICATE_FILE;
		String privateKeyFile = AWS_IOT_PRIVATE_KEY_FILE;

		SampleUtil.KeyStorePasswordPair pair = SampleUtil.getKeyStorePasswordPair(certificateFile, privateKeyFile);
		this.client = new AWSIotMqttClient(clientEndpoint, clientId, pair.keyStore, pair.keyPassword);

		try {
			client.connect();
			MinecraftServer currentServer = ServerLifecycleHooks.getCurrentServer();
			//currentServer.getWo
			// https://github.com/aws/aws-iot-device-sdk-java/tree/master/aws-iot-device-sdk-java-samples/src/main/java/com/amazonaws/services/iot/client/sample/pubSub
			AWSIotTopic topic = new TopicListener(AWS_IOT_INBOUND_TOPIC, AWS_IOT_INBOUND_TOPIC_QOS, currentServer);
			client.subscribe(topic, true);
		} catch (Exception e) {
			LOGGER.error("Error connecting to IoT", e);
		}
	}

    private void doClientStuff(final FMLClientSetupEvent event) {
        // do something that can only be done on the client
        LOGGER.info("Got game settings {}", event.getMinecraftSupplier().get().options);
    }

    private void enqueueIMC(final InterModEnqueueEvent event)
    {
        // some example code to dispatch IMC to another mod
        InterModComms.sendTo("examplemod", "helloworld", () -> { LOGGER.info("Hello world from the MDK"); return "Hello world";});
    }

    private void processIMC(final InterModProcessEvent event)
    {
        // some example code to receive and process InterModComms from other mods
        LOGGER.info("Got IMC {}", event.getIMCStream().
                map(m->m.getMessageSupplier().get()).
                collect(Collectors.toList()));
    }
    // You can use SubscribeEvent and let the Event Bus discover methods to call
    @SubscribeEvent
    public void onServerStarting(FMLServerStartingEvent event) {
        // do something when the server starts
        LOGGER.info("HELLO from server starting");
    }

	@SubscribeEvent
	public void makeJumpHigher(LivingEvent.LivingJumpEvent event) {
		/*LOGGER.info("Jump event");

		try {
			client.publish(ExampleMod.AWS_IOT_TOPIC, "{ \"msg\": \"direto do minecraft\", \"action\": \"jump\"}");
		} catch (Exception e) {
			LOGGER.error("Error publishing event to IoT", e);
		}*/

//		try {
//			World world = event.getEntityLiving().getCommandSenderWorld();
//			world.setRainLevel(new Random().nextFloat());
//			world.setThunderLevel(new Random().nextFloat());
//			LOGGER.info("Turning rain on");
//		} catch (Exception e) {
//			LOGGER.error("Turning rain on", e);
//		}
	}
	int lamp =0;
	@SubscribeEvent
	public void blockBreak(BlockEvent.BreakEvent event) {
		LOGGER.info("Block break");

		try {
			//client.publish(ExampleMod.AWS_IOT_TOPIC, "{ \"msg\": \"direto do minecraft\", \"action\": \"block-break\"}");
			client.publish(ExampleMod.AWS_IOT_TOPIC, "" + lamp);
			LOGGER.info("Turning " + (lamp == 0 ? "off" : "on") + " lamp");

			lamp = lamp==0 ? 1 : 0;
		} catch (Exception e) {
			LOGGER.error("Error publishing event to IoT", e);
		}
	}

    // You can use EventBusSubscriber to automatically subscribe events on the contained class (this is subscribing to the MOD
    // Event bus for receiving Registry Events)
    @Mod.EventBusSubscriber(bus=Mod.EventBusSubscriber.Bus.MOD)
    public static class RegistryEvents {
        @SubscribeEvent
        public static void onBlocksRegistry(final RegistryEvent.Register<Block> blockRegistryEvent) {
            // register a new block here
            LOGGER.info("HELLO from Register Block");
        }
    }
}
