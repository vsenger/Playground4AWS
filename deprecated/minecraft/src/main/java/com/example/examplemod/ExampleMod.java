package com.example.examplemod;

import com.ibm.icu.util.Output;
import net.minecraft.world.item.Item;
import net.minecraft.world.level.block.Block;
import net.minecraft.world.level.block.Blocks;
import net.minecraftforge.common.MinecraftForge;
import net.minecraftforge.event.RegistryEvent;
import net.minecraftforge.event.entity.living.LivingEvent;
import net.minecraftforge.event.world.BlockEvent.BreakEvent;
import net.minecraftforge.event.world.ChunkDataEvent;
import net.minecraftforge.event.world.WorldEvent;
import net.minecraftforge.eventbus.api.SubscribeEvent;
import net.minecraftforge.fml.InterModComms;
import net.minecraftforge.fml.common.Mod;

import net.minecraftforge.fml.event.lifecycle.FMLCommonSetupEvent;
import net.minecraftforge.fml.event.lifecycle.InterModEnqueueEvent;
import net.minecraftforge.fml.event.lifecycle.InterModProcessEvent;
import net.minecraftforge.fmlserverevents.FMLServerStartingEvent;
import net.minecraftforge.fml.javafmlmod.FMLJavaModLoadingContext;
import net.minecraftforge.registries.DeferredRegister;
import net.minecraftforge.registries.ForgeRegistries;
import org.apache.logging.log4j.LogManager;
import org.apache.logging.log4j.Logger;

//import com.amazonaws.services.iot.client.AWSIotException;
//import com.amazonaws.services.iot.client.AWSIotTimeoutException;
//import com.amazonaws.services.iot.client.sample.pubSub.PublishSubscribeSample;

import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.stream.Collectors;

// The value here should match an entry in the META-INF/mods.toml file
@Mod(ExampleMod.MODID)
public class ExampleMod
{
    public static final String MODID = "examplemod";

    // Directly reference a log4j logger.
    private static final Logger LOGGER = LogManager.getLogger();
    public static final DeferredRegister<Block> BLOCKS = DeferredRegister.create(ForgeRegistries.BLOCKS, ExampleMod.MODID);
    public static final DeferredRegister<Item> ITEMS = DeferredRegister.create(ForgeRegistries.ITEMS, ExampleMod.MODID);

    public static int relay_status = 0;
    @SubscribeEvent
    public void worldEvent(WorldEvent event) {
        //System.out.println("----->World " + event);
        if(event instanceof ChunkDataEvent) {
            //System.out.println(((ChunkDataEvent) event).getWorld().da);
        }
        /*if (event.block != Blocks.diamond_ore) {
            return;
        }*/
    }
    @SubscribeEvent
    public void explode(BreakEvent event) {
        System.out.println("----->Explode " + event);

        /*if (event.block != Blocks.diamond_ore) {
            return;
        }*/
    }
    public ExampleMod() {
        // Register the setup method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::setup);
        // Register the enqueueIMC method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::enqueueIMC);
        // Register the processIMC method for modloading
        FMLJavaModLoadingContext.get().getModEventBus().addListener(this::processIMC);

        // Register ourselves for server and other game events we are interested in
        MinecraftForge.EVENT_BUS.register(this);
        String args[] = {"-clientEndpoint a2p4fyajwx9lux-ats.iot.us-east-1.amazonaws.com",
                "-clientId 12343443322",
                "-certificateFile /Users/vsenger/minecraft-iot/8f2b2f776911332a0fea819064421830e592b55f32c4a6262918700841fc5c32-certificate.pem.crt",
                "-privateKeyFile /Users/vsenger/minecraft-iot/8f2b2f776911332a0fea819064421830e592b55f32c4a6262918700841fc5c32-private.pem.key"
        };
        /*try {
            PublishSubscribeSample.main(args);
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }*/
    }
    private void setup(final FMLCommonSetupEvent event)
    {
        // some preinit code
        LOGGER.info("HELLO FROM PREINIT");
        LOGGER.info("DIRT BLOCK >> {}", Blocks.DIRT.getRegistryName());
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
                map(m->m.messageSupplier().get()).
                collect(Collectors.toList()));
    }
    @SubscribeEvent
    public void makeJumpHigher(LivingEvent.LivingJumpEvent event) {
        System.out.println("-----> pulou :" + event);
        System.out.println("pulou");
        
        //event.getEntityLiving().motionY = 10;

        //event.get
        //event.getEntityLiving().flyingSpeed*=5;
        //event.getEntity().set
    }
    // You can use SubscribeEvent and let the Event Bus discover methods to call
    @SubscribeEvent
    public void onServerStarting(FMLServerStartingEvent event) {
        // do something when the server starts
        LOGGER.info("HELLO from server starting");
    }
    @SubscribeEvent
    public void blockBreak(BreakEvent event) {

        System.out.println("Block break!");
        try {
            System.out.println("Block break here also!");
            Socket socket = new Socket("localhost", 9999);
            OutputStream output = socket.getOutputStream();
            OutputStreamWriter writer = new OutputStreamWriter(new PrintStream(output));
            writer.write("" + relay_status);
            writer.flush();
            relay_status = relay_status == 0 ? 1 : 0;
            socket.close();
        }
        catch(Exception e) {
            e.printStackTrace();
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
