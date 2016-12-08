package com.tubbsoffun.cloud_tub_app;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.ImageButton;
import android.widget.ProgressBar;
import android.widget.TextView;

import com.fasterxml.jackson.databind.JsonNode;
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

import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.Iterator;

public class MainActivity extends AppCompatActivity {

    /** Pubnub Settings */
    private final String PublishKey = "demo";
    private final String SubscribeKey = "demo";
    private final String Channel = "WINC1500_85:DB";

    private final String TempUnitC = "ºC";
    private final String TempUnitF = "ºF";

    private final String waterTempToken = "water_temp";
    private final String setTempToken = "temp_setpoint";

    private static final int PROGRESS = 0x1;

    private final int MaxTempC = 40;
    private final int MaxTempF = 104;
    private final int MinTempC = 20;
    private final int MinTempF = 68;

    private PNStatus pnStatus;

    private ProgressBar mProgress;
    private int mProgressStatus = 0;

    private TextView mConnStatusTxt;
    private TextView mWaterTempTxt;
    private TextView mSetTempTxt;

    private ImageButton mTempUpBtn;
    private ImageButton mTempDownBtn;

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

        // Create a thread for updating the connection status progress bar
        /*final Handler handler = new Handler();
        Runnable runnable = new Runnable() {
            @Override
            public void run() {
                if (pnStatus.getCategory() == PNStatusCategory.PNConnectedCategory) {
                    mProgress.setVisibility(View.INVISIBLE);
                    mProgressStatus = 0;
                }
                else {
                    mProgressStatus = ++mProgressStatus % 100;
                    mProgress.setProgress(mProgressStatus);
                }
                handler.postDelayed(this, 500);
            }
        };
        runnable.run();*/

        PNConfiguration pnConfiguration = new PNConfiguration();

        pnConfiguration.setSubscribeKey(SubscribeKey);
        pnConfiguration.setPublishKey(PublishKey);
        pnConfiguration.setSecure(false);
        //PubNub pubnub = new PubNub(pnConfiguration);
        PubNub pubnub = new PubNub(pnConfiguration);

        pubnub.addListener(new SubscribeCallback() {
            @Override
            public void status(PubNub pubnub, PNStatus status) {
                pnStatus = status;
                if (status.getCategory() == PNStatusCategory.PNUnexpectedDisconnectCategory) {
                    // Radio / connectivity lost
                    mProgress.setVisibility(View.VISIBLE);
                    mConnStatusTxt.setText("Connecting...");
                }
                else if (status.getCategory() == PNStatusCategory.PNConnectedCategory) {
                    // Connect event. Do stuff like publish here.
                    mProgress.setVisibility(View.INVISIBLE);
                    mConnStatusTxt.setText("Connected");
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
                    mConnStatusTxt.setText("Connected");
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
                        JsonNode node = message.getMessage();
                        Iterator itr = node.elements();

                        while (itr.hasNext()) {
                            if (node.asText().equals(waterTempToken)) {
                                String waterTempString = node.path(waterTempToken).asText();
                                waterTempString.concat(TempUnitF);
                                mWaterTempTxt.setText(waterTempString);
                            }
                            node = (JsonNode) itr.next();
                        }

                        // Message has been received on channel group stored in message.getChannel()
                        //String water_temp = message.getMessage().with(waterTempToken).asText();
                        //mWaterTempTxt.setText(water_temp + TempUnitF);
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
}
