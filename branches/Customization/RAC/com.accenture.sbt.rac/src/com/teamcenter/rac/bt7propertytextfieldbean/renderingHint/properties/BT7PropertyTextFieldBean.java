package com.teamcenter.rac.bt7propertytextfieldbean.renderingHint.properties;

import java.util.Map;

import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Text;
import org.eclipse.ui.forms.widgets.FormToolkit;

import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCProperty;
import com.teamcenter.rac.kernel.TCPropertyDescriptor;
import com.teamcenter.rac.kernel.TCSession;
import com.teamcenter.rac.kernel.TCTextService;
import com.teamcenter.rac.util.Registry;
import com.teamcenter.rac.util.controls.TextControl;
import com.teamcenter.rac.viewer.stylesheet.beans.TextAreaPropertyBean;

public class BT7PropertyTextFieldBean extends TextAreaPropertyBean
{

	private static final long serialVersionUID = 1L;
	protected TCTextService textService;
	protected TCSession session;
	private Registry appRegistry = Registry.getRegistry(this);
	private TextControl m_textControl;
	
	public BT7PropertyTextFieldBean(TextControl paramTextControl)
	  {
		 super(paramTextControl);
		 this.m_textControl = paramTextControl;
		 
	  }
	
	public BT7PropertyTextFieldBean(FormToolkit paramFormToolkit, Composite paramComposite, boolean paramBoolean, Map paramMap)
	  {
	   super(paramFormToolkit,paramComposite,paramBoolean,paramMap);
	  
	  }
	 @Override
	public void load(TCProperty arg0) throws Exception {
		// TODO Auto-generated method stub
		 
		super.load(arg0);
		setSize(50, 15);
			
	}
	@Override
	public TCProperty getPropertyToSave(TCComponent arg0) throws Exception {
		// TODO Auto-generated method stub
		return super.getPropertyToSave(arg0);
	}
	@Override
	public boolean isPropertyModified(TCProperty arg0) throws Exception {
		// TODO Auto-generated method stub
		return true;
	}
	@Override
	public void load(TCPropertyDescriptor arg0) throws Exception {
		// TODO Auto-generated method stub
		super.load(arg0);
	}
	@Override
	public void setModifiable(boolean arg0) {
		// TODO Auto-generated method stub
		super.setModifiable(arg0);
	}
	@Override
	public void setUIFValue(Object arg0) {
		// TODO Auto-generated method stub
		super.setUIFValue(arg0);
	}
	@Override
	public void save(TCComponent arg0) throws Exception {
		// TODO Auto-generated method stub
		super.save(arg0);
	}
	@Override
	public void save(TCProperty arg0) throws Exception {
		// TODO Auto-generated method stub
		super.save(arg0);
	}
	@Override
	public Text getTextField() {
		// TODO Auto-generated method stub
		return super.getTextField();
	}
	@Override
	public void setTextField(TextControl arg0) {
		// TODO Auto-generated method stub
		super.setTextField(arg0);
	}
	@Override
	public void setText(String arg0) {
		// TODO Auto-generated method stub
		super.setText(arg0);
	}
	@Override
	public void setLayoutData(GridData arg0) {
		// TODO Auto-generated method stub
		super.setLayoutData(arg0);
	}
	@Override
	public Object getEditableValue() {
		// TODO Auto-generated method stub
		return super.getEditableValue();
	}
}
