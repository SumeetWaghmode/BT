package com.teamcenter.rac.commands.paste;

import com.teamcenter.rac.aif.AbstractAIFUIApplication;
import com.teamcenter.rac.aif.kernel.AIFComponentContext;
import com.teamcenter.rac.aif.kernel.InterfaceAIFComponent;
import com.teamcenter.rac.kernel.TCComponent;
import com.teamcenter.rac.kernel.TCException;
import com.teamcenter.rac.util.Registry;
//

import javax.swing.JOptionPane;


public class CustomPasteOperation extends PasteOperation {

	private static AbstractAIFUIApplication paramAbstractAIFUIApplication;
	private static TCComponent paramTCComponent;
	private static InterfaceAIFComponent paramInterfaceAIFComponent;
	private static String paramString;
	private TCComponent parentComp;
	private AIFComponentContext[] contextArray;
	private InterfaceAIFComponent[] childrenComps;
	private String contextString;
	private boolean openAfterPasteFlag;
	private Object operationResultNew;
	private boolean statusUse;
	private Registry myRegistry;
	
	 public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent paramInterfaceAIFComponent, String paramString)
	  {
	    super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramArrayOfInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString, boolean paramBoolean)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramArrayOfInterfaceAIFComponent,paramString);
		  this.paramTCComponent = paramTCComponent;
		  this.paramString = paramString;
		  /* parentComp = this.paramTCComponent;
		  contextString = this.paramString;*/
	  }
	  
	  public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString)
	  {
		  super(paramTCComponent,paramArrayOfInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, AIFComponentContext paramAbstractAIFUIApplication1)
	  {
		  super(paramAbstractAIFUIApplication,paramAbstractAIFUIApplication1);
	  }
	  
	  public CustomPasteOperation(AIFComponentContext paramAIFComponentContext)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AIFComponentContext paramAIFComponentContext, boolean paramBoolean)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, AIFComponentContext[] paramArrayOfAIFComponentContext)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(AIFComponentContext[] paramArrayOfAIFComponentContext)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	  public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString, Object paramObject, boolean paramBoolean)
	  {
		  super(paramAbstractAIFUIApplication,paramTCComponent,paramInterfaceAIFComponent,paramString);
	  }
	  
	 /* public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramArrayOfInterfaceAIFComponent, Object paramObject, boolean paramBoolean1, boolean paramBoolean2)
	  {
		  super(paramTCComponent,paramTCComponent,paramArrayOfInterfaceAIFComponent,paramArrayOfInterfaceAIFComponent);
	  }*/
	  
	@Override
	public void executeOperation() throws TCException {
		// TODO Auto-generated method stub
		myRegistry = Registry.getRegistry(this);
		
		String ParObjType = paramTCComponent.getProperty("object_type");
		if (paramString.equals(myRegistry.getString("PART_MATERIAL_REL")) && (ParObjType.contains("Revision")))
		{
			String message = myRegistry.getString("INFO_MESSAGE");
			try 
			{
				 JOptionPane.showMessageDialog(null,message);
			} 
			catch (Exception e) 
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			//return null;
		}
		else
		{
			super.executeOperation();
		}
		
	}
	
	
//
//	public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent paramInterfaceAIFComponent, String paramString)
//	  {
//	    AIFComponentContext localAIFComponentContext = new AIFComponentContext(paramTCComponent, paramInterfaceAIFComponent, paramString);
//	    this.contextArray = new AIFComponentContext[] { localAIFComponentContext };
//	  }
//	  
//	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString)
//	  {
//	    this.parentComp = paramTCComponent;
//	    this.childrenComps = paramArrayOfInterfaceAIFComponent;
//	    this.contextString = paramString;
//	  }
//	  
//	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString, boolean paramBoolean)
//	  {
//	    this.parentComp = paramTCComponent;
//	    this.childrenComps = paramArrayOfInterfaceAIFComponent;
//	    this.contextString = paramString;
//	    this.openAfterPasteFlag = paramBoolean;
//	  }
//	  
//	  public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString)
//	  {
//	    this.parentComp = paramTCComponent;
//	    this.childrenComps = paramArrayOfInterfaceAIFComponent;
//	    this.contextString = paramString;
//	  }
//	  
//	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, AIFComponentContext paramAIFComponentContext)
//	  {
//	    this.contextArray = new AIFComponentContext[] { paramAIFComponentContext };
//	  }
//	  
//	  public CustomPasteOperation(AIFComponentContext paramAIFComponentContext)
//	  {
//	    this.contextArray = new AIFComponentContext[] { paramAIFComponentContext };
//	  }
//	  
//	  public CustomPasteOperation(AIFComponentContext paramAIFComponentContext, boolean paramBoolean)
//	  {
//	    this.contextArray = new AIFComponentContext[] { paramAIFComponentContext };
//	    this.openAfterPasteFlag = paramBoolean;
//	  }
//	  
//	  public CustomPasteOperation(AbstractAIFUIApplication paramAbstractAIFUIApplication, AIFComponentContext[] paramArrayOfAIFComponentContext)
//	  {
//	    this(paramArrayOfAIFComponentContext);
//	  }
//	  
//	  public CustomPasteOperation(AIFComponentContext[] paramArrayOfAIFComponentContext)
//	  {
//	    this.contextArray = paramArrayOfAIFComponentContext;
//	  }
//	  
//	  public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString, Object paramObject, boolean paramBoolean)
//	  {
//	    this(paramTCComponent, paramArrayOfInterfaceAIFComponent, paramString, paramObject, paramBoolean, false);
//	  }
//	  
//	  public CustomPasteOperation(TCComponent paramTCComponent, InterfaceAIFComponent[] paramArrayOfInterfaceAIFComponent, String paramString, Object paramObject, boolean paramBoolean1, boolean paramBoolean2)
//	  {
//	    this.parentComp = paramTCComponent;
//	    this.childrenComps = paramArrayOfInterfaceAIFComponent;
//	    this.contextString = paramString;
//	    this.operationResultNew = paramObject;
//	    this.statusUse = paramBoolean1;
//	    this.openAfterPasteFlag = paramBoolean2;
//	  }
//
//	@Override
//	public void executeOperation() throws Exception {
//		// TODO Auto-generated method stub
//		String ParType = parentComp.getProperty("object_type");
//		
//		if (contextString.equals("Mat1HasAlternateMaterial"))
//		{
//			System.out.println("I am here ...");
//		}
//		
//	}
  
}
