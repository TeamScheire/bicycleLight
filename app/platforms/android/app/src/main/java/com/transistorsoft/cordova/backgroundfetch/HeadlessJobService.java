package com.transistorsoft.cordova.backgroundfetch;

/**
 * Created by chris on 2018-01-22.
 */

import android.annotation.TargetApi;
import android.app.job.JobParameters;
import android.app.job.JobService;
import android.util.Log;

import com.transistorsoft.tsbackgroundfetch.BackgroundFetch;
import com.transistorsoft.tsbackgroundfetch.FetchJobService;

/**
 * Created by chris on 2018-01-17.
 */

@TargetApi(21)
public class HeadlessJobService extends JobService {

    @Override
    public boolean onStartJob(final JobParameters params) {

        BackgroundFetch adapter = BackgroundFetch.getInstance(getApplicationContext());

        if (adapter.isMainActivityActive()) {
            return true;
        }

        adapter.registerCompletionHandler(new FetchJobService.CompletionHandler() {
            @Override
            public void finish() {
                Log.d(BackgroundFetch.TAG, "HeadlessJobService jobFinished");
                jobFinished(params, false);
            }
        });

        Log.d(BackgroundFetch.TAG, "HeadlessJobService onStartJob");
        new BackgroundFetchHeadlessTask().onFetch(getApplicationContext());
        return true;
    }
    @Override
    public boolean onStopJob(JobParameters params) {
        Log.d(BackgroundFetch.TAG, "JobService onStopJob");
        jobFinished(params, false);
        return true;
    }
}
