package it.rikiji.android.walrusdict;

import java.util.ArrayList;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;

public class CustomListAdapter extends ArrayAdapter<String[]> {
	private ArrayList<String[]> items;

	public CustomListAdapter(Context context, int textViewResourceId,
			ArrayList<String[]> items) {
		super(context, textViewResourceId, items);
		this.items = items;
	}

	@Override
	public View getView(int position, View convertView, ViewGroup parent) {
		View v = convertView;
		if (v == null) {
			LayoutInflater vi = (LayoutInflater) this.getContext().getSystemService(Context.LAYOUT_INFLATER_SERVICE);
			v = vi.inflate(R.layout.list_item, null);
		}

		TextView entry_dict = (TextView) v.findViewById(R.id.entry_dict);
		TextView entry_from = (TextView) v.findViewById(R.id.entry_from);
		TextView entry_to = (TextView) v.findViewById(R.id.entry_to);
		entry_dict.setText(this.items.get(position)[0]);
		entry_from.setText(this.items.get(position)[1]);
		entry_to.setText(this.items.get(position)[2]);
		return v;
	}
}
