package it.rikiji.android.walrusdict;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.util.ArrayList;
import java.util.List;

import com.google.gson.Gson;

import android.os.AsyncTask;
import android.os.Bundle;
import android.app.Activity;
import android.text.Editable;
import android.text.TextWatcher;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.ListView;
import android.widget.Toast;

public class MainActivity extends Activity {

	EditText input;
	CustomListAdapter adapter;
	public static final String PROT = "http";
	public static final String HOST = "192.168.1.201";
	public static final String PORT = "3000";
	protected ArrayList<String[]> data;
	protected String[][] dataOrig;

	@Override
	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		this.input = (EditText) findViewById(R.id.input);
		input.addTextChangedListener(new TextWatcher() {
			public void afterTextChanged(Editable s) {
				new DownloadData().execute(input.getText().toString());
				Log.d("onTextChanged", input.getText().toString());
			}

			public void beforeTextChanged(CharSequence s, int start, int count,
					int after) {
			}

			public void onTextChanged(CharSequence s, int start, int before,
					int count) {
			}
		});

		ListView list = (ListView) findViewById(R.id.list);

		this.data = new ArrayList<String[]>();
		this.adapter = new CustomListAdapter(this, R.layout.list_item, data);
		list.setAdapter(this.adapter);

		list.setOnItemClickListener(new OnItemClickListener() {
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				String[] e = MainActivity.this.data.get(position);
				new PushCard().execute(e[1] + "<br>" + e[0], e[2]);
				Toast.makeText(getApplicationContext(), "Sending to anki...",
						Toast.LENGTH_SHORT).show();
			}
		});
	}

	private String format(String[] input, String delimiter) {
		StringBuilder sb = new StringBuilder();
		for (String value : input) {
			sb.append(value);
			sb.append(delimiter);
		}
		int length = sb.length();
		if (length > 0) {
			// Remove the extra delimiter
			sb.setLength(length - delimiter.length());
		}
		return sb.toString();
	}

	private Menu getListAdapter() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		getMenuInflater().inflate(R.menu.activity_main, menu);
		return true;
	}

	public static String root_url() {
		return PROT + "://" + HOST + ":" + PORT + "/";
	}

	private class DownloadData extends AsyncTask {

		private final String url = root_url() + "query?val=";

		protected void onPostExecute(Object result) {

			Gson gson = new Gson();
			Log.d("debug", "fetched:  " + result);
			MainActivity.this.dataOrig = gson.fromJson((String) result,
					String[][].class);
			MainActivity.this.data.clear();
			int i = 0;
			for (String[] x : MainActivity.this.dataOrig) {
				MainActivity.this.data.add(x);
			}
			MainActivity.this.adapter.notifyDataSetChanged();
		}

		protected Object doInBackground(Object... params) {

			String val = params[0].toString();
			String result = "[]";
			try {
				URL apiUrl = new URL(this.url + val);
				HttpURLConnection conn = (HttpURLConnection) apiUrl
						.openConnection();
				conn.setConnectTimeout(30000);
				conn.setReadTimeout(30000);
				conn.setRequestMethod("GET");
				conn.connect();
				InputStream is = conn.getInputStream();
				BufferedReader rd = new BufferedReader(
						new InputStreamReader(is), 4096);
				String line;
				StringBuilder sb = new StringBuilder();
				while ((line = rd.readLine()) != null) {
					sb.append(line);
				}
				rd.close();
				result = sb.toString();
			} catch (Exception e) {
				Log.d("error", e.getMessage());
			}
			return result;
		}
	}

	private class PushCard extends AsyncTask {

		private final String url = root_url() + "anki?";

		protected void onPostExecute(Object result) {

			if (result != null) {
				Gson gson = new Gson();
				Log.d("debug", "fetched:  " + result);
				String[] res = gson.fromJson((String) result, String[].class);
				Toast.makeText(getApplicationContext(), res[0],
						Toast.LENGTH_LONG).show();
			} else {
				Toast.makeText(getApplicationContext(), "error",
						Toast.LENGTH_LONG).show();
			}
		}

		protected Object doInBackground(Object... params) {

			String result = "[]";
			try {
				URL apiUrl = new URL(this.url + "front="
						+ URLEncoder.encode((String) params[0], "utf-8")
						+ "&back="
						+ URLEncoder.encode((String) params[1], "utf-8"));
				HttpURLConnection conn = (HttpURLConnection) apiUrl
						.openConnection();
				conn.setConnectTimeout(30000);
				conn.setReadTimeout(30000);
				conn.setRequestMethod("GET");
				conn.connect();
				InputStream is = conn.getInputStream();
				BufferedReader rd = new BufferedReader(
						new InputStreamReader(is), 4096);
				String line;
				StringBuilder sb = new StringBuilder();
				while ((line = rd.readLine()) != null) {
					sb.append(line);
				}
				rd.close();
				result = sb.toString();
			} catch (Exception e) {
				Log.d("error", e.getMessage());
			}
			return result;
		}
	}

}
