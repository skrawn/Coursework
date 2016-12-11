package com.tubbsoffun.cloud_tub_app;

import android.inputmethodservice.KeyboardView;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.JsonWriter;
import android.util.Log;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.fasterxml.jackson.core.Base64Variant;
import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonParser;
import com.fasterxml.jackson.core.JsonPointer;
import com.fasterxml.jackson.core.JsonStreamContext;
import com.fasterxml.jackson.core.JsonToken;
import com.fasterxml.jackson.core.ObjectCodec;
import com.fasterxml.jackson.core.SerializableString;
import com.fasterxml.jackson.core.TreeNode;
import com.fasterxml.jackson.core.Version;
import com.fasterxml.jackson.databind.JsonNode;
import com.fasterxml.jackson.databind.SerializerProvider;
import com.fasterxml.jackson.databind.deser.std.StringArrayDeserializer;
import com.fasterxml.jackson.databind.jsontype.TypeSerializer;
import com.fasterxml.jackson.databind.node.JsonNodeType;
import com.pubnub.api.*;
import com.pubnub.api.PubNub;
import com.pubnub.api.callbacks.PNCallback;
import com.pubnub.api.callbacks.SubscribeCallback;
import com.pubnub.api.enums.PNStatusCategory;
import com.pubnub.api.models.consumer.PNPublishResult;
import com.pubnub.api.models.consumer.PNStatus;
import com.pubnub.api.models.consumer.pubsub.PNMessageResult;
import com.pubnub.api.models.consumer.pubsub.PNPresenceEventResult;

import org.json.*;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.lang.ref.WeakReference;
import java.math.BigDecimal;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;
import java.util.concurrent.Semaphore;

public class MainActivity extends AppCompatActivity {

    /** Pubnub Settings */
    PubNub pubnub;
    private final String PublishKey = "demo";
    private final String SubscribeKey = "demo";
    private final String Channel = "WINC1500_85:DB";

    private final String TempUnitC = " ºC";
    private final String TempUnitF = " ºF";

    private final String deviceToken = "device";
    private final String waterTempToken = "water_temp";
    private final String degreesFToken = "degress_F";
    private final String getTempToken = "set_temp";
    private final String setTempToken = "temp_setpoint";

    private static final int MAX_AVAILABLE = 100;
    private final Semaphore ui_sem = new Semaphore(MAX_AVAILABLE, true);

    private boolean degrees_F = true;

    private static final int PROGRESS = 0x1;

    private final int MaxTempC = 40;
    private final int MaxTempF = 104;
    private final int MinTempC = 20;
    private final int MinTempF = 68;

    private PNStatus pnStatus;

    // UI Elements
    private ProgressBar mProgress;
    private int mProgressStatus = 0;

    private TextView mConnStatusTxt;
    private TextView mWaterTempTxt;
    private TextView mSetTempTxt;

    private ImageButton mTempUpBtn;
    private ImageButton mTempDownBtn;

    private String waterTempString = "0";
    private String setTempString = "0";
    private String connStatusString = "Connecting...";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.constraint_test);
        //Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        //setSupportActionBar(toolbar);

        // Get UI elements
        mProgress = (ProgressBar) findViewById(R.id.connProgress);
        mConnStatusTxt = (TextView) findViewById(R.id.connStatusTxt);
        mWaterTempTxt = (TextView) findViewById(R.id.waterTempTxt);
        mSetTempTxt = (TextView) findViewById(R.id.setTempTxt);

        mTempDownBtn = (ImageButton) findViewById(R.id.tempDownButton);
        mTempDownBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int currentTemp = Integer.parseInt(setTempString);
                int min_temp;
                if (degrees_F) {
                    min_temp = MinTempF;
                }
                else {
                    min_temp = MinTempC;
                }
                if (currentTemp > min_temp) {
                    send_new_setpoint(currentTemp - 1);
                }
            }
        });

        mTempUpBtn = (ImageButton) findViewById(R.id.tempUpButton);
        mTempUpBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                int currentTemp = Integer.parseInt(setTempString);
                int max_temp;
                if (degrees_F) {
                    max_temp = MaxTempF;
                }
                else {
                    max_temp = MaxTempC;
                }

                if (currentTemp < max_temp) {
                    send_new_setpoint(currentTemp + 1);
                }
            }
        });

        PNConfiguration pnConfiguration = new PNConfiguration();

        pnConfiguration.setSubscribeKey(SubscribeKey);
        pnConfiguration.setPublishKey(PublishKey);
        pnConfiguration.setSecure(false);
        pubnub = new PubNub(pnConfiguration);

        pubnub.addListener(new SubscribeCallback() {
            @Override
            public void status(PubNub pubnub, PNStatus status) {
                pnStatus = status;
                if (status.getCategory() == PNStatusCategory.PNUnexpectedDisconnectCategory) {
                    // Radio / connectivity lost
                    mProgress.setVisibility(View.VISIBLE);
                    set_conn_status("Connecting...");
                }
                else if (status.getCategory() == PNStatusCategory.PNConnectedCategory) {
                    // Connect event. Do stuff like publish here.
                    mProgress.setVisibility(View.INVISIBLE);
                    set_conn_status("Connected");
                    if (status.getCategory() == PNStatusCategory.PNConnectedCategory) {
                        pubnub.publish().channel(Channel).message("app_connect")
                                .async(new PNCallback<PNPublishResult>() {
                            @Override
                            public void onResponse(PNPublishResult result, PNStatus status) {
                                if (!status.isError()) {
                                    // Message published successfully to channel
                                }
                                else {
                                    // Message failed to publish successfully
                                }
                            }
                        });
                    }
                }
                else if (status.getCategory() == PNStatusCategory.PNReconnectedCategory) {
                    // Happens when radio connectivity is lost then regained.
                    mProgress.setVisibility(View.INVISIBLE);
                    set_conn_status("Connected");
                }
                else if (status.getCategory() == PNStatusCategory.PNDecryptionErrorCategory) {
                    // Handle message decryption error. Probably client configured to encrypt messages
                    // and on live data feed it received plain text.
                }
            }

            @Override
            public void message(PubNub pubnub, PNMessageResult message) {
                if (message.getChannel() != null) {
                    if (message.getMessage() != null) {
                        String temp;
                        JsonNode node = message.getMessage();
                        if (node.has(degreesFToken)) {
                            degrees_F = node.path(degreesFToken).asBoolean();
                        }

                        if (node.has(waterTempToken)) {
                            temp = node.path(waterTempToken).asText();
                            set_water_temperature(temp);
                        }

                        if (node.has(getTempToken)) {
                            temp = node.path(getTempToken).asText();
                            set_setpoint_temperature(temp);
                        }
                    }
                }
                else {
                    // Message has been received on channel stored in message.getSubscription()
                }
                /*
                    log the following items with your favorite logger
                        - message.getMessage()
                        - message.getSubscription()
                        - message.getTimetoken()
                */
            }

            @Override
            public void presence(PubNub pubnub, PNPresenceEventResult presence) {

            }
        });

        pubnub.subscribe().channels(Arrays.asList(Channel)).execute();

        // Start the UI thread
        ui_thread();

        /*FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });*/
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    private void send_new_setpoint(int new_setpoint)
    {
        pubnub.publish().channel(Channel).message(Arrays.asList("temp_setpoint", String.valueOf(new_setpoint))).
                async(new PNCallback<PNPublishResult>() {
            @Override
            public void onResponse(PNPublishResult result, PNStatus status) {
                if (!status.isError()) {

                }
                else {

                }
            }
        });
    }

    private void set_water_temperature(String new_temp)
    {
        waterTempString = new_temp;
        ui_sem.release();
    }

    private void set_setpoint_temperature(String new_temp)
    {
        setTempString = new_temp;
        ui_sem.release();
    }

    private void set_conn_status(String new_status)
    {
        connStatusString = new_status;
        ui_sem.release();
    }

    private void ui_thread()
    {
        Thread thread = new Thread() {
            public void run() {
                Boolean b = true;
                try {
                    do {
                        ui_sem.acquire();
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                String temp_units;
                                if (degrees_F)
                                    temp_units = TempUnitF;
                                else
                                    temp_units = TempUnitC;

                                mWaterTempTxt.setText(waterTempString.concat(temp_units));
                                mConnStatusTxt.setText(connStatusString);
                                mSetTempTxt.setText(setTempString.concat(temp_units));
                            }
                        });
                    }
                    while (b);
                }
                catch (Exception e) {
                    e.printStackTrace();
                }
            };
        };
        thread.start();
    }
}
