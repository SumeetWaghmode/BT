// @<COPYRIGHT>@
// ==================================================
// Copyright 2016.
// Siemens Product Lifecycle Management Software Inc.
// All Rights Reserved.
// ==================================================
// @<COPYRIGHT>@

package com.sbt.awc.ccr.internal.commands;

import java.sql.Timestamp;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.logging.Level;

import com.google.gwt.user.client.rpc.AsyncCallback;
import com.google.inject.Inject;
import com.sbt.awc.ccr.internal.NameTokens;
import com.siemens.splm.client.workflow.published.WorkflowService;
import com.siemens.splm.clientfx.base.published.IMessageContext;
import com.siemens.splm.clientfx.base.published.IMessageContext.OkayCancelKeys;
import com.siemens.splm.clientfx.base.published.IMessageService;
import com.siemens.splm.clientfx.base.published.IMessageService.IMessageHandlerComplete;
import com.siemens.splm.clientfx.base.published.NavigationOption;
import com.siemens.splm.clientfx.base.resources.i18n.BaseMessages;
import com.siemens.splm.clientfx.kernel.clientmodel.published.IModelObject;
import com.siemens.splm.clientfx.kernel.clientmodel.published.IProperty;
import com.siemens.splm.clientfx.kernel.published.IPropertyName;
import com.siemens.splm.clientfx.kernel.published.ISession;
import com.siemens.splm.clientfx.kernel.soa.published.ISOAResponse;
import com.siemens.splm.clientfx.tcui.utils.published.ModelUtils;
import com.siemens.splm.clientfx.ui.commands.published.AbstractCommandHandler;

/**
 *  StartCCR command handler.
 */
public class StartCCRCommandHandler
    extends AbstractCommandHandler
{
	/** message service */
    @Inject
    private IMessageService m_msgSvc;
    
    /** session */
    @Inject
    private ISession m_session;
    
    /**
     * Constructor
     */
    public StartCCRCommandHandler()
    {
        super( NameTokens.CMD_StartCCR );
    }

    @Override
    public SelectionOptionEnum getCommandSelectionOption() {
    	// this is a cell command handler, not tied to selection context.
        return SelectionOptionEnum.None;
    }
    
    @Override
    protected void doExecute()
    {
        /**
         * In this method, you'll typically get the selection & execute the command on it.
         */
    	
    	// check if current logged in user is infodba
    	final IModelObject userSession = ModelUtils.getLoggedInUser();
    	
    	if( userSession != null )
	    {
    		IModelObject[] objs = new IModelObject[1];
            objs[0] = userSession;
    		ModelUtils.ensurePropertiesLoaded( objs, new String[] { IPropertyName.USER_NAME }, new AsyncCallback<Void>() {

				@Override
				public void onFailure(Throwable caught) {
					m_msgSvc.error(caught);
					
				}

				@Override
				public void onSuccess(Void result) {

					String userName="";
					IProperty property = userSession.getPropertyObject( IPropertyName.USER_NAME );
		            if( property != null )
		            {
		                userName = property.getValue().toString();
		            }
				            
			    	if(userName.equalsIgnoreCase("infodba")){
			    		m_msgSvc.error("Infodba is an installation user account and should not be used to create Workspace Objects.");
			    		return;
			    	}
			    	
			    	// confirm dialog    	
			    	List<NavigationOption> navigationOptions = new ArrayList<>();
			        navigationOptions.add( new NavigationOption( OkayCancelKeys.OK, BaseMessages.INSTANCE.ok() ) );
			        navigationOptions.add( new NavigationOption( OkayCancelKeys.CANCEL, BaseMessages.INSTANCE.cancel() ) );
			        
			    	IMessageContext msgContext = m_msgSvc.createMessageContext();
			    	msgContext.init( Level.WARNING, "Do you want to start a Common Component Request (CCR) ?", true,null,
			    			navigationOptions );
			    	msgContext.setHideOnTimeout( false );
			    	
			    	m_msgSvc.report(msgContext, new IMessageHandlerComplete() {
						
						@Override
						public void handled(IMessageContext context, Object selectedNavigationOption) {
							 if( selectedNavigationOption != null )
			                 {
			                     NavigationOption navigationOption = (NavigationOption) selectedNavigationOption;
			                     if( (OkayCancelKeys) navigationOption.getKey() == OkayCancelKeys.OK )
			                     {
			                    	 String userName = "";
			                    	 IProperty property = userSession.getPropertyObject( IPropertyName.USER_NAME );
			     		            if( property != null )
			     		            {
			     		                userName = property.getValue().toString();
			     		            }
									
									// prepare date format
									Timestamp time = new Timestamp(new Date().getTime());
									String datestring = time.toString().substring(0, 10);
									
			                    	 String workflowName = "CCR_" + userName + "_"+datestring;
			                    	 IModelObject[] attachements = new IModelObject[0];
			                    	 
			                    	 WorkflowService.createInstance(true, "", workflowName, "", "", "CC Request", attachements, new AsyncCallback<ISOAResponse>() {
			                 			
			                 			@Override
			                 			public void onSuccess(ISOAResponse result) {
			                 				m_msgSvc.notify("Common Component Request (CCR) successfully started. Please check your Worklist and enter the details of the request."
			                 						, true);
			                 			}
			                 			
			                 			@Override
			                 			public void onFailure(Throwable caught) {
			                 				m_msgSvc.error(caught);				
			                 			}
			                 		});
			                     }
			                 }
						}
					});

					
				}
			});
        }
    	    }
}
